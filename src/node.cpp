#include "node.h"

using namespace PUT::SBH;

Node::Node(Oligo & o) : value(o) {};

void Node::connect(Node * n, int weight) {
  adjacent.insert(std::make_pair(n, weight));
}
