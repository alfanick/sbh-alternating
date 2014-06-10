#include "sequencer.h"

using namespace PUT::SBH;

Sequencer::Sequencer() {};

Sequencer::Sequencer(Chip &chip, int n, int k, Sequence start) {
  feed(chip, n, k, start);
}

void Sequencer::feed(Chip &chip, int n, int k, Sequence start) {
  this->chip = chip;
  this->n = n;
  this->k = k;
  this->oligo_length = k*2-1;
  Sequence first = start.substr(0, k*2-1);
  Sequence second = start.substr(1, k*2-1);

  for (int i = 1; i < first.length(); i+=2) first[i] = Nucleotide::X;

  for (int i = 1; i < second.length(); i+=2) second[i] = Nucleotide::X;

  this->start = make_pair(first,second);
  results.clear();
}

void Sequencer::search(Sequence current_path, Sequence last_path, bool odd) {

  std::vector<Edge> cands_list = candidates(current_path, last_path);

  for(auto chosen : cands_list) {
    Sequence chosen_seq = chosen.first->value->sequence;
    // std::cerr << "Adding " << chosen.first->value->sequence << " to " << *current_path << "\n";
    current_path += chosen_seq.substr(chosen_seq.length() - 2*(chosen.second));
    if (current_path.length() == n-1 && last_path.length() == n-1) {
      Sequence *seq = odd ? join(last_path, current_path) : join(current_path, last_path);
      std::cerr << *seq << std::endl;
      results.push_back(seq);
      // remove_last();
      // break;
    } else {
      chosen.first->occurence -= 1;
      search(last_path, current_path, !odd);
      chosen.first->occurence += 1;
    }
  }
}

void Sequencer::run() {
  results.clear();

  verify_list.clear();

  for (auto &val : chip[1])
    verify_list.push_back(&(val.first.sequence));

  build_graph();
  print_graph();

  search(start.first, start.second, false);

}

//   even_path = start.first;
//   odd_path = start.second;
// 
//   even_path_edges.push_back(std::make_pair(graph[start.first],0));
//   odd_path_edges.push_back(std::make_pair(graph[start.second],0));
// 
//   current_path = &even_path;
//   last_path = &odd_path;
//   current_path_edges = &even_path_edges;
//   last_path_edges = &odd_path_edges;
// 
//   //int iter = 0;
// 
//   while (1) {
// 
//     std::vector<Edge> cands_list = candidates();
// 
//     Sequence last_o = last_oligo(*current_path);
// 
//     if (cands_list.size() > 0) {
//       Edge chosen = cands_list[0];
//       Sequence chosen_seq = chosen.first->value->sequence;
//       std::cerr << "Adding " << chosen.first->value->sequence << " to " << *current_path << "\n";
//       *current_path += chosen_seq.substr(chosen_seq.length() - 2*(chosen.second));
//       current_path_edges->push_back(chosen);
//       chosen.first->occurence -= 1;
//     } else if (current_path_edges->size() > 0) {
//       remove_last();
//     }
// 
//     if (current_path->length() == n-1 && last_path->length() == n-1) {
//       std::cout << *join() << std::endl;
//       results.push_back(join());
//       remove_last();
//       // break;
//     }
// 
//     std::swap(current_path, last_path);
//     std::swap(current_path_edges, last_path_edges);
//   }
// }


void Sequencer::remove_last() {
  Edge last_edge = *(--current_path_edges->end());
  last_edge.first->occurence += 1;
  std::cerr << "Removing " << last_edge.first->value->sequence << " from " << *current_path << "\n";
  current_path_edges->erase(--current_path_edges->end());
  Node* last_node = (*(--current_path_edges->end())).first;

  if (last_node->adjacent.find(last_edge) != last_node->adjacent.end())
    last_node->adjacent.erase(last_node->adjacent.find(last_edge));

  *current_path = current_path->substr(0,current_path->length() - last_edge.second*2);
  std::cerr << "Current path is " << *current_path << " now\n";
}

Sequence* Sequencer::join(Sequence even_path, Sequence odd_path) {
  Sequence* result = new Sequence("");

  for (int i = 0; i <= odd_path.length()+1; ++i) {
    *result += (i % 2) ? odd_path[i-1] : even_path[i];
  }

  return result;
}


std::vector<Edge> Sequencer::candidates(Sequence current_path, Sequence last_path) {
  Sequence last = last_oligo(current_path);
  Node* node = graph[last];
  std::vector<Edge> cands;

  if (current_path.length() <= last_path.length() && node != NULL) {
    for (auto adj : node->adjacent) {
      Node* next_node = adj.first;
      int weight = adj.second;

      if (weight < INT_MAX) {   // Skip non-overlapping
        Sequence possible = last_path.substr(last_path.length() - oligo_length + 2);
        Sequence next_seq = next_node->value->sequence;
        possible += next_seq[next_seq.size() - 1 - 2*(weight-1)];

        if (next_node->occurence > 0 && verify(possible))
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
  for (auto &iter : chip[0])
    graph.insert(std::make_pair(iter.first.sequence, new Node(&iter.first, iter.second)));

  // Connect vertices
  for (auto &i1 : chip[0]) {
    Oligo o1 = i1.first;

    for (auto &i2 : chip[0]) {
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

  for (auto &i : chip[0]) {
    std::cerr << i.first.sequence << ":\n";
    Node* node = graph[i.first.sequence];

    for (auto &j : node->adjacent)
      std::cerr << "\t" << j.first->value->sequence << "(" << j.second << ")\n";
  }

  std::cerr << "*****************************************\n";
}

Sequence Sequencer::last_oligo(Sequence s) {
  std::cerr << "Looking for last oligo for sequence " << s << std::endl;
  return s.substr(s.length() - oligo_length);
}

