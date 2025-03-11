#include <iostream>
#include "api_bridge.hpp"

int main() {
    try {
        // Initialize the bridge with your API base URL
        APIBridge api("http://127.0.0.1:8000");

        // Example of triggering an event (same as your curl example)
        APIBridge::TriggerEvent event{
            .type = "GoToPoiEvent",
            .payloadName = "GotoPoiPayload",
            .payload = {
                {"entity", 2},
                {"target", "patientRoom"}
            }
        };

        // Try to trigger the event
        auto result = api.triggerEvent(event);
        if (result.success) {
            std::cout << "Event triggered successfully! (Status: " << result.status_code << ")\n";
        } else {
            std::cout << "Failed to trigger event (Status: " << result.status_code << ")\n";
            std::cout << "Error: " << result.error_message << "\n";
        }

        // Try to query a component (for example, ent 1, component "position")
        auto query_result = api.queryComponent(1, "position");
        if (query_result.success) {
            std::cout << "Received component info:\n" << query_result.data.dump(2) << "\n";
        } else {
            std::cout << "Failed to query component (Status: " << query_result.status_code << ")\n";
            std::cout << "Error: " << query_result.error_message << "\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
