#include <curl/curl.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include <memory>

template <typename... Args>
std::string string_format(const std::string &format, Args... args)
{
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
    if (size_s <= 0)
        throw std::runtime_error("Error during formatting.");

    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);

    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *response)
{
    size_t totalSize = size * nmemb;
    response->append((char *)contents, totalSize);

    return totalSize;
}

class Bridge
{
private:
    std::string url;
    std::string error;

    std::string postRequest(std::string &postData)
    {
        CURL *curl;
        CURLcode res;
        std::string response;

        // Initialize cURL
        curl = curl_easy_init();
        if (curl)
        {
            // Set to verbose mode
            // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            // Set the URL for the POST request
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

            // Specify that this is a POST request
            curl_easy_setopt(curl, CURLOPT_POST, 1L);

            // Set the POST data
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

            // Set the content type header
            struct curl_slist *headers = nullptr;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // Set the callback function to capture the response
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            // Perform the request
            res = curl_easy_perform(curl);

            // Check for errors
            if (res != CURLE_OK)
                error = curl_easy_strerror(res);

            // Clean up
            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);
        }
        else
            error = "Failed to initialize cURL.";

        if (!error.empty())
            return "";

        return response;
    }

public:
    Bridge(const std::string &url) : url(url) {}

    void gotopoi(const std::string &poi)
    {
        std::cout << url << std::endl;

        std::string data = string_format("{\"type\":\"GoToPoiEvent\",\"payloadName\":\"GotoPoiPayload\",\"payload\":{\"entity\":2,\"target\":\"%s\"}}", poi.c_str());
        std::cout << data << std::endl;

        std::string result = postRequest(data);
        if (!error.empty())
            std::cerr << error << std::endl;
        else
            std::cout << result << std::endl;
    }
};
