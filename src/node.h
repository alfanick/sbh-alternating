#ifndef PUT_SBH_ALT_NODE_H_
#define PUT_SBH_ALT_NODE_H_

#include <set>
#include <utility>
#include <map>
#include "oligo.h"

namespace PUT {
  namespace SBH {
    class Node;
    typedef std::pair<Node*, int> Edge;
    typedef std::map<std::string, Node*> Graph;
    class Node {
     private:
      struct edgescomp {
        bool operator()(const Edge &a, const Edge &b) const {
          if (a.second != b.second) return a.second < b.second;

          return a.first < b.first;
        }
      };
     public:
      const oligo* value;
      int occurence;
      std::set<Edge, edgescomp> adjacent;

      Node(const Oligo*, int);

      void connect(Node*, int);
    };
  }
}

#endif
