#ifndef PUT_SBH_ALT_SEQUENCER_H_
#define PUT_SBH_ALT_SEQUENCER_H_

#include "spectrum.h"
#include "oligo.h"
#include "sequence.h"
#include "node.h"
#include <vector>
#include <iostream>
#include <utility>

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
      Sequencer(Chip &, int, int, Sequence);


      /**
       * Feeds sequencer algorithm with data
       */
      void feed(Chip &, int, int, Sequence);

      /**
       * Runs algorithm
       */
      void run();

      /**
       * Returns possible results list
       */
      std::vector<Sequence> get_results();

     private:
      int n,                                // Result sequence length
          k,                                // Determines oligonucleotide length
          oligo_length;                     // Oligonucletide length
      std::vector<const Sequence* > results,// Possible results list
          verify_list;      // Verification list based on second part of chip
      Chip chip;                            // Array of 2 spectrum objects
      std::pair<Sequence, Sequence> start;  // Two first starting oligos (even and odd)
      Graph graph;                          // Map of nodes
      Sequence even_path, odd_path,         // Even and odd path
               *current_path,               // Currently updated path
               *last_path;                  // Last updated path
      std::vector<Edge>   even_path_edges,  // Edges list of even path
          odd_path_edges,  // Edges list of odd path
          *current_path_edges,
          *last_path_edges;


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

      /**
       * Prints graph to output
       */
      void print_graph();

      /**
       * Generates candidate vertices list for last nucleotide in
       * current path
       */
      std::vector<Edge> candidates();


      /**
       * Verify given oligo using currently built path and second
       * part of Chip Spectrum
       */
      bool verify(Sequence &);

      /**
       * Cuts given sequence to return last oligo basing on oligo
       * length
       */
      Sequence last_oligo(Sequence);

      /**
       * Joins even and odd path to create result
       */
      Sequence* join();
    };
  }
}

#endif

