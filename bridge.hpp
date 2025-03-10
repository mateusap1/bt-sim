#ifndef BRIDGE_HPP
#define BRIDGE_HPP

#include <string>
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

using json = nlohmann::json;

// Callback to capture the response body
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

class Bridge {
private:
    std::string base_url;
    CURL* curl;

    struct Response {
        long status_code;
        std::string body;
    };

    // This function makes the HTTP call. Notice that payload_str is declared in the outer scope
    Response makeRequest(const std::string& endpoint, const std::string& method, 
                         const json& payload = json()) {
        if (!curl) {
            throw std::runtime_error("CURL not initialized");
        }

        std::string url = base_url + endpoint;
        std::string response_string;
        
        curl_easy_reset(curl);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Declare payload_str outside the if-block so that it stays in scope until after curl_easy_perform.
        std::string payload_str;
        if (method == "POST") {
            payload_str = payload.dump();
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload_str.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);

            // Debug output
            std::cout << "Sending POST request to: " << url << std::endl;
            std::cout << "Payload: " << payload_str << std::endl;
        } else if (method == "GET") {
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        }

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            throw std::runtime_error("CURL request failed: " + 
                std::string(curl_easy_strerror(res)));
        }

        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        return Response{http_code, response_string};
    }

public:
    // Structures for trigger event and result.
    struct TriggerEvent {
        std::string type;
        std::string payloadName;
        json payload;
    };

    struct TriggerResult {
        bool success;
        long status_code;
        std::string error_message;
    };

    Bridge(const std::string& url) : base_url(url) {
        curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Failed to initialize CURL");
        }
    }

    ~Bridge() {
        if (curl) {
            curl_easy_cleanup(curl);
        }
    }

    TriggerResult triggerEvent(const TriggerEvent& event) {
        // Build the request payload exactly as in the working curl command.
        json request_payload = {
            {"type", event.type},
            {"payloadName", event.payloadName},
            {"payload", event.payload}
        };

        try {
            auto response = makeRequest("/trigger", "POST", request_payload);

            if (response.status_code != 200) {
                std::string error_msg;
                try {
                    if (!response.body.empty()) {
                        json error_json = json::parse(response.body);
                        error_msg = error_json.dump(2);
                    } else {
                        error_msg = "No error message provided";
                    }
                } catch (const json::parse_error& e) {
                    error_msg = response.body;
                }
                return TriggerResult{false, response.status_code, error_msg};
            }

            return TriggerResult{true, response.status_code, ""};
        } catch (const std::exception& e) {
            return TriggerResult{false, 0, e.what()};
        }
    }

    // Structures for query result.
    struct QueryResult {
        bool success;
        long status_code;
        json data;
        std::string error_message;
    };

    QueryResult queryComponent(int ent, const std::string& component) {
        try {
            std::string endpoint = "/query/" + std::to_string(ent) + "/" + component;
            auto response = makeRequest(endpoint, "GET");

            // Parse the response JSON.
            json parsed;
            try {
                parsed = json::parse(response.body);
            } catch (const json::parse_error& e) {
                return QueryResult{false, response.status_code, json(), "Invalid JSON response"};
            }

            // If the parsed JSON indicates an error (for example, when the entity is not found),
            // mark the query as failed.
            if (parsed.is_array() && parsed.size() >= 2 &&
                parsed[0].is_object() && parsed[0].contains("status") &&
                parsed[0]["status"] == "Error") {
                return QueryResult{false, response.status_code, parsed, parsed[0]["error"].get<std::string>()};
            }

            return QueryResult{true, response.status_code, parsed, ""};
        } catch (const std::exception& e) {
            return QueryResult{false, 0, json(), std::string(e.what())};
        }
    }

    TriggerResult moveToPOI(const std::string& poi) {
        TriggerEvent event{
            .type = "GoToPoiEvent",
            .payloadName = "GotoPoiPayload",
            .payload = {
                {"entity", 2},
                {"target", poi}
            }
        };

        return triggerEvent(event);
    }
};

#endif // API_BRIDGE_HPP
