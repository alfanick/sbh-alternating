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
  std::cout << "Building verify list\n";
  for(auto val : chip[1]) {
    std::cout << "Veirfy: " << &(val.first.sequence) << std::endl;
    verify_list.push_back(&(val.first.sequence));
  }

  build_graph();
  print_graph();

  even_path = start.first;
  odd_path = start.second;

  current_path = &even_path;
  last_path = &odd_path;
  current_path_edges = &even_path_edges;
  last_path_edges = &odd_path_edges;

  int iter = 0;

  while(1) {

    std::vector<Edge> cands_list = candidates();

    Sequence last_o = last_oligo(*current_path);
    std::cout << "Last oligo: " << last_o << "\n";
    if(cands_list.size() > 0) {
      std::cout << "dupa" << std::endl;
      Edge chosen = cands_list[0];
      Sequence chosen_seq = chosen.first->value->sequence;
      *current_path += chosen_seq.substr(chosen_seq.length() - 2*(chosen.second));
      current_path_edges->push_back(chosen);
      chosen.first->occurence -= 1;
      std::cerr << "Adding " << chosen.first->value->sequence << " to " << *current_path << "\n";
    }
    else if(current_path_edges->size() > 0){
      Edge last_edge = *(--current_path_edges->end());
      last_edge.first->occurence += 1;
      std::cerr << "Removing " << last_edge.first->value->sequence << " from " << *current_path << "\n";
      current_path_edges->erase(--current_path_edges->end());
      Node * last_node = (*(--current_path_edges->end())).first;
      last_node->adjacent.erase(last_node->adjacent.find(last_edge));
      *current_path = current_path->substr(0,current_path->length() - last_edge.second*2);
      std::cerr << "Current path is " << *current_path << " now\n";
    }

    if(current_path->length() == n-1) {
      std::cout << "Sukces\n" << *current_path << "\n " << *last_path << "\n";
      break;
    }

    std::swap(current_path, last_path);
    std::swap(current_path_edges, last_path_edges);

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


std::vector<Edge> Sequencer::candidates() {
  Sequence last = last_oligo(*current_path);
  Node * node = graph[last];
  std::vector<Edge> cands;

  if(current_path->length() <= last_path->length() && node != NULL) {
    for(auto adj : node->adjacent) {
      Node * next_node = adj.first;
      int weight = adj.second;
      if(weight < INT_MAX) {    // Skip non-overlapping
        Sequence possible = last_path->substr(last_path->length() - oligo_length + 2);
        Sequence next_seq = next_node->value->sequence;
        possible += next_seq[next_seq.size() - 1 - 2*(weight-1)];
        std::cout << "veryfing " << possible << " with result " << verify(possible) << std::endl;
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
  std::cerr << "*********** Adjacency graph ************\n";
  for (auto& i : chip[0]) {
    std::cerr << i.first.sequence << ":\n";
    Node *node = graph[i.first.sequence];
    for (auto& j : node->adjacent)
      std::cerr << "\t" << j.first->value->sequence << "(" << j.second << ")\n";
  }
  std::cerr << "*****************************************\n";
}

Sequence Sequencer::last_oligo(Sequence s) {
  return s.substr(s.length() - oligo_length);
}
