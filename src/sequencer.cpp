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
  this->oligo_length = k*2-1;
  Sequence first = start.substr(0, start.length()-1);
  Sequence second = start.substr(1);
  for(int i = 1; i < first.length(); i+=2) first[i] = Nucleotide::X;
  for(int i = 1; i < second.length(); i+=2) second[i] = Nucleotide::X;
  this->start = make_pair(first,second);
  results.clear();
}

void Sequencer::run() {
  results.clear();

  verify_list.clear();
  for(auto val : chip[1]) 
    verify_list.push_back(val.first.sequence);

  // for(auto ver : verify_list)
  //   std::cout << "Verification: " << ver << "\n";
  // std::cout << "\n";

  build_graph();

  even_path = start.first;
  odd_path = start.second;

  current_path = &even_path;
  last_path = &odd_path;

  std::cout << "start: " << start.first << "," << start.second << "\n";

  int iter = 0;

  while(1) {

    std::vector<Sequence> cands_list = candidates();
    
    std::cout << "current path: " << *current_path << "\n";
    // for(auto cand : cands_list)
    //   std::cout << "Candidate: " << cand << "\n";

    std::cout << std::endl;

    std::swap(current_path, last_path);
    if(iter++ > 0)
      break;
  }
}


std::vector<Sequence> Sequencer::candidates() {
  Sequence last = current_path->substr(current_path->length() - oligo_length);
  Node * node = graph[last];
  std::vector<Sequence> cands;
  if(current_path->length() <= last_path->length() && node != NULL) {
    for(auto adj : node->adjacent) {
      Node * next_node = adj.first;
      int weight = adj.second;
      if(weight < INT_MAX) {    // Skip non-overlapping
        Sequence possible = last_path->substr(last_path->length() - oligo_length + 2);
        Sequence next_seq = next_node->value.sequence;
        possible += next_seq[next_seq.size() - 1 - 2*(weight-1)];
        if(next_node->occurence > 0 && verify(possible))
          cands.push_back(next_node->value.sequence);
      }
    }
  }
  return cands;
}

bool Sequencer::verify(Sequence &possible) {
  return (std::find(verify_list.begin(), verify_list.end(), possible) != verify_list.end());
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
      if(i1 != i2) {
        Oligo o2 = i2->first;
        graph[o1.sequence]->connect(graph[o2.sequence], o1.max_overlap(o2));
      }
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
