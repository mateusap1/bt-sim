#include <iostream>
#include <string>

#include <behaviortree_cpp_v3/behavior_tree.h>
#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/blackboard.h>

#include "bridge.cpp"

class MoveTo : public BT::SyncActionNode
{
private:
    Bridge bridge;

public:
    MoveTo(const std::string &name, const BT::NodeConfiguration &config) : BT::SyncActionNode(name, config), bridge("http://127.0.0.1:8000/trigger") {}

    static BT::PortsList providedPorts()
    {
        return {BT::InputPort<std::string>("destination")};
    }

    BT::NodeStatus tick() override
    {
        BT::Optional<std::string> res = getInput<std::string>("destination");
        if (!res) {
            throw BT::RuntimeError("Missing required input [destination]: ", res.error());
        }
        std::string destination = res.value();

        bridge.gotopoi(destination);
        std::cout << "Moving to " << destination << std::endl;

        return BT::NodeStatus::SUCCESS;
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
        if (!res) {
            throw BT::RuntimeError("Missing required input [item]: ", res.error());
        }
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
        if (!res) {
            throw BT::RuntimeError("Missing required input [item]: ", res.error());
        }
        std::string item = res.value();

        std::cout << "Dropping item " << item << std::endl;

        return BT::NodeStatus::SUCCESS;
    }
};