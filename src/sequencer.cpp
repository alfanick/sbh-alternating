#include "sequencer.h"

using namespace PUT::SBH;

Sequencer::Sequencer() {};

Sequencer::Sequencer(Chip & chip, int n, int k, Sequence start) {
  feed(chip, n, k, start);
}

void Sequencer::feed(Chip & chip, int n, int k, Sequence start) {
  this->chip = chip;
  this->n = n;
  this->k = k;
  Sequence first = start.substr(0, start.length()-1);
  Sequence second = start.substr(1);
  for(int i = 1; i < first.length(); i+=2) first[i] = Nucleotide::X;
  for(int i = 1; i < second.length(); i+=2) second[i] = Nucleotide::X;
  this->start = make_pair(first,second);
  results.clear();
}

void Sequencer::run() {
  results.clear();
  build_graph();
  print_graph();

  current_path = 0;

  even_path = start.first;
  odd_path = start.second;

  std::cout << "start: " << start.first << "," << start.second << "\n";

  while(1) {
    Sequence *current;
    // Even path
    if(current_path % 2 == 0)
      current = &even_path;
    else
      current = &odd_path;

    current_path++;
  }
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

void Sequencer::print_graph() {
  std::cout << "*********** Adjacency graph ************\n";
  for(auto i = chip[0].begin(); i != chip[0].end(); ++i) {
    std::cout << i->first.sequence << ":\n";
    Node *node = graph[i->first.sequence];
    for(auto j = node->adjacent.begin(); j != node->adjacent.end(); ++j)
      std::cout << "\t" << j->first->value.sequence << "(" << j->second << ")\n";
  }
  std::cout << "*****************************************\n";
}
