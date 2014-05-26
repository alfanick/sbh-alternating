#include "node.h"

using namespace PUT::SBH;

Node::Node(Oligo o, int number) : value(o), occurence(number) {};

void Node::connect(Node * n, int weight) {
  adjacent.insert(std::make_pair(n, weight));
}
