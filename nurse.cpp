#include "behaviortree_cpp_v3/bt_factory.h"

#include "nodes.cpp"

int main()
{
    // We use the BehaviorTreeFactory to register our custom nodes
  BT::BehaviorTreeFactory factory;
  Bridge bridge = Bridge("");

  // The recommended way to create a Node is through inheritance.
  factory.registerNodeType<MoveTo>("MoveTo", bridge);
  factory.registerNodeType<Grab>("Grab");
  factory.registerNodeType<Drop>("Drop");

  // IMPORTANT: when the object "tree" goes out of scope, all the 
  // TreeNodes are destroyed
  auto tree = factory.createTreeFromFile("./bts/hospital.xml");

  // To "execute" a Tree you need to "tick" it.
  // The tick is propagated to the children based on the logic of the tree.
  // In this case, the entire sequence is executed, because all the children
  // of the Sequence return SUCCESS.
  tree.tickRootWhileRunning();

  return 0;
}
