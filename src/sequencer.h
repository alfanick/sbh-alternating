#ifndef PUT_SBH_ALT_SEQUENCER_H_
#define PUT_SBH_ALT_SEQUENCER_H_

#include "spectrum.h"
#include "oligo.h"
#include "sequence.h"
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
        int n,                        // Result sequence length
            k;                        // Determines oligonucleotyde length
        std::vector<Sequence> results;     // Possible results list
        Chip chip;                    // Array of 2 spectrum objects 

    };
  }
}

#endif

