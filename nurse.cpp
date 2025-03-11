#include "behaviortree_cpp_v3/bt_factory.h"
#include "nodes.cpp"  // Contains MoveTo, Grab, and Drop definitions

int main()
{
    BT::BehaviorTreeFactory factory;

    // Register custom nodes without extra parameters.
    factory.registerNodeType<MoveTo>("MoveTo");
    factory.registerNodeType<Grab>("Grab");
    factory.registerNodeType<Drop>("Drop");

    // Create the tree from XML.
    auto tree = factory.createTreeFromFile("./bts/hospital.xml");

    // Execute the tree by ticking it until it finishes.
    tree.tickRootWhileRunning();

    return 0;
}
