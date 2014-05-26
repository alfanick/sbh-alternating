#ifndef PUT_SBH_ALT_SEQUENCER_H_
#define PUT_SBH_ALT_SEQUENCER_H_

#include "spectrum.h"
#include "oligo.h"
#include "sequence.h"
#include "node.h"
#include <vector>

namespace PUT {
  namespace SBH {
    /**
     * Sequencing algorithm
     * */
    class Sequencer {
      public:
        /**
         * Blank constructor
         */
        Sequencer();

        /**
         * Constructor with data to feed algorithm
         */
        Sequencer(Chip &, int, int);
        

        /**
         * Feeds sequencer algorithm with data
         */
        void feed(Chip &, int, int);

        /**
         * Runs algorithm
         */
        void run();

        /**
         * Returns possible results list
         */
        std::vector<Sequence> get_results();

      private:
        int n,                          // Result sequence length
            k;                          // Determines oligonucleotyde length
        std::vector<Sequence> results;  // Possible results list
        Chip chip;                      // Array of 2 spectrum objects 
        Oligo start[2];                 // Two first starting oligos (even and odd)
        Graph graph;                    // Map of nodes

        /**
         * Builds graph on top of given spectrum
         * - Each oligo represents one node
         * - Edges between nodes represents overlapping - the bigger
         *   weight is, the smaller is max possible overlap between
         *   two oligos
         * - Each node contains information about number of oligos
         *   in spectrum
         */
        void build_graph();
    };
  }
}

#endif

