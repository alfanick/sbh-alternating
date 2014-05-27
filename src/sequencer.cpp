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
    verify_list.push_back(&(val.first.sequence));

  build_graph();
  print_graph();

  even_path = start.first;
  odd_path = start.second;

  current_path = &even_path;
  last_path = &odd_path;
  current_path_nodes = &even_path_nodes;
  last_path_nodes = &odd_path_nodes;

  int iter = 0;

  while(1) {

    std::vector<std::pair<Node *, int> > cands_list = candidates();

    Sequence last_o = last_oligo(*current_path);
    if(cands_list.size() > 0) {
      std::pair<Node *, int> chosen = cands_list[0];
      Sequence chosen_seq = chosen.first->value->sequence;
      *current_path += chosen_seq.substr(chosen_seq.length() - 2*(chosen.second));
      current_path_nodes->push_back(chosen.first);
      chosen.first->occurence -= 1;
    }
    else {
      // Remove last node
    }

    if(current_path->length() == n-1) {
      std::cout << "Sukces\n" << *current_path << "\n " << *last_path << "\n";
      break;
    }

    std::swap(current_path, last_path);
    std::swap(current_path_nodes, last_path_nodes);

    if(iter++ > 4) {
      std::cout << "Fail\n" << *current_path << "\n " << *last_path << "\n";
      std::cout << join() << "\n";
      break;
    }
  }
}

Sequence Sequencer::join() {
  Sequence result = "";
  for(int i = 0; i <= odd_path.length(); ++i) {
    result += (i % 2) ? odd_path[i-1] : even_path[i];
  }
  return result;
}


std::vector<std::pair<Node *, int> > Sequencer::candidates() {
  Sequence last = last_oligo(*current_path);
  Node * node = graph[last];
  std::vector<std::pair<Node *, int> > cands;

  if(current_path->length() <= last_path->length() && node != NULL) {
    for(auto adj : node->adjacent) {
      Node * next_node = adj.first;
      int weight = adj.second;
      if(weight < INT_MAX) {    // Skip non-overlapping
        Sequence possible = last_path->substr(last_path->length() - oligo_length + 2);
        Sequence next_seq = next_node->value->sequence;
        possible += next_seq[next_seq.size() - 1 - 2*(weight-1)];
        if(next_node->occurence > 0 && verify(possible))
          cands.push_back(adj);
      }
    }
  }
  return cands;
}

bool Sequencer::verify(Sequence &possible) {
  return (std::find_if(verify_list.begin(), verify_list.end(), [possible](const Sequence* self) {
    return *self == possible;
  }) != verify_list.end());
}

void Sequencer::build_graph() {
  graph.clear();

  // Add vertices to graph
  for (auto& iter : chip[0])
    graph.insert(std::make_pair(iter.first.sequence, new Node(&iter.first, iter.second)));

  // Connect vertices
  for (auto& i1 : chip[0]) {
    Oligo o1 = i1.first;
    for (auto& i2 : chip[0]) {
      if (i1 != i2) {
        Oligo o2 = i2.first;
        int max_overlap = o1.max_overlap(o2);
        if (max_overlap != INT_MAX)
          graph[o1.sequence]->connect(graph[o2.sequence], max_overlap);
      }
    }
  }
}

void Sequencer::print_graph() {
  std::cout << "*********** Adjacency graph ************\n";
  for (auto& i : chip[0]) {
    std::cout << i.first.sequence << ":\n";
    Node *node = graph[i.first.sequence];
    for (auto& j : node->adjacent)
      std::cout << "\t" << j.first->value->sequence << "(" << j.second << ")\n";
  }
  std::cout << "*****************************************\n";
}

Sequence Sequencer::last_oligo(Sequence s) {
  return s.substr(s.length() - oligo_length);
}
