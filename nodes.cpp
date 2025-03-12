#include <iostream>
#include <string>

#include <behaviortree_cpp_v3/behavior_tree.h>
#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/blackboard.h>

#include "bridge.hpp"

class MoveTo : public BT::StatefulActionNode
{
public:
    // Now the Bridge is initialized internally.
    MoveTo(const std::string &name, const BT::NodeConfiguration &config)
        : BT::StatefulActionNode(name, config),
          bridge("http://127.0.0.1:8000") // Adjust the URL/params as needed.
    {}

    static BT::PortsList providedPorts()
    {
        return {BT::InputPort<std::string>("destination")};
    }

    BT::NodeStatus onStart() override
    {
        std::string destination = parseDestination();
        auto result = bridge.moveToPOI(destination);
        if (result.success) {
            std::cout << "Moving to " << destination << std::endl;
            return BT::NodeStatus::RUNNING;
        } else {
            std::cout << "Failed to trigger event (Status: " << result.status_code << ")\n";
            std::cout << "Error: " << result.error_message << "\n";
            return BT::NodeStatus::FAILURE;
        }
    }

    BT::NodeStatus onRunning() override 
    {
        auto query_result = bridge.queryComponent(2, "position");
        if (query_result.success) {
            json position = query_result.data["position"];

            if (closeToPosition(514, 240, position["x"], position["y"], 20))
                return BT::NodeStatus::SUCCESS;
            return BT::NodeStatus::RUNNING;
        } else {
            std::cout << "Failed to query component (Status: " << query_result.status_code << ")\n";
            std::cout << "Error: " << query_result.error_message << "\n";
            return BT::NodeStatus::FAILURE;
        }
    }

    void onHalted() override {}

private:
    Bridge bridge;

    bool closeToPosition(double targetX, double targetY, double actualX, double actualY, double offset) {
        // Calculate the Euclidean distance between the two points.
        double distance = std::hypot(targetX - actualX, targetY - actualY);
        return distance <= offset;
    }

    std::string parseDestination()
    {
        BT::Optional<std::string> res = getInput<std::string>("destination");
        if (!res)
            throw BT::RuntimeError("Missing required input [destination]: ", res.error());
        return res.value();
    }
};

class Grab : public BT::SyncActionNode
{
public:
    Grab(const std::string &name, const BT::NodeConfiguration &config) : BT::SyncActionNode(name, config) {}

    static BT::PortsList providedPorts()
    {
        return {BT::InputPort<std::string>("item")};
    }

    BT::NodeStatus tick() override
    {
        BT::Optional<std::string> res = getInput<std::string>("item");
        if (!res)
            throw BT::RuntimeError("Missing required input [item]: ", res.error());
        std::string item = res.value();

        std::cout << "Grabbing item " << item << std::endl;

        return BT::NodeStatus::SUCCESS;
    }
};

class Drop : public BT::SyncActionNode
{
public:
    Drop(const std::string &name, const BT::NodeConfiguration &config) : BT::SyncActionNode(name, config) {}

    static BT::PortsList providedPorts()
    {
        return {BT::InputPort<std::string>("item")};
    }

    BT::NodeStatus tick() override
    {
        BT::Optional<std::string> res = getInput<std::string>("item");
        if (!res)
            throw BT::RuntimeError("Missing required input [item]: ", res.error());
        std::string item = res.value();

        std::cout << "Dropping item " << item << std::endl;

        return BT::NodeStatus::SUCCESS;
    }
};
