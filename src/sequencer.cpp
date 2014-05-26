#include "sequencer.h"

using namespace PUT::SBH;

Sequencer::Sequencer() {};

Sequencer::Sequencer(Chip & chip, int n, int k) {
  feed(chip, n, k);
}

void Sequencer::feed(Chip & chip, int n, int k) {
  this->chip = chip;
  this->n = n;
  this->k = k;
  results.clear();
}

void Sequencer::run() {
  build_graph();
}

void Sequencer::build_graph() {
  graph.clear();

  // Add vertices to graph
  for(auto iter = chip[0].begin(); iter != chip[0].end(); ++iter)
    graph.insert(std::make_pair(iter->first.sequence, new Node(iter->first, iter->second)));

  // Connect vertices
  for(auto i1 = chip[0].begin(); i1 != chip[0].end(); ++i1) {
    Oligo o1 = i1->first;
    for(auto i2 = chip[0].begin(); i2 != chip[0].end(); ++i2) {
      Oligo o2 = i2->first;
      graph[o1.sequence]->connect(graph[o2.sequence], o1.max_overlap(o2));
    }
  }
}
