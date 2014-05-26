#ifndef PUT_SBH_ALT_NODE_H_
#define PUT_SBH_ALT_NODE_H_

#include <set>
#include <utility>
#include "oligo.h"

namespace PUT {
  namespace SBH {
    class Node {
      private:
        struct edgescomp {
          bool operator()(const std::pair<Node *, int> &a, const std::pair<Node *, int> b) const {
            return a < b;
          }
        };
      public:
        oligo value;
        std::set<std::pair<Node *, int>, edgescomp> adjacent;

        Node(Oligo &);

        void connect(Node *, int);
    };
  }
}

#endif
