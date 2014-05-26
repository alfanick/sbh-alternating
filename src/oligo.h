#ifndef PUT_SBH_ALT_OLIGO_H_
#define PUT_SBH_ALT_OLIGO_H_

#include <string>
#include <vector>
#include <queue>
#include <climits>

namespace PUT {
  namespace SBH {
    /**
     * Oligonucleotyde built with alternating chip rules
     * */
    class Oligo {
      public:
        enum type {
          CLASSIC = 0,
          SPECIAL
        };

        unsigned int length;
        std::string sequence;
        char type;

        Oligo();
        Oligo(std::string);
        Oligo(const Oligo &);

        /**
         * Sets sequence string, its length and type
         */
        void set_sequence(const std::string);

        /**
         * Alias to set_sequence
         * */
        Oligo& operator=(const std::string);

        /**
         * Compares oligos basing on alternating chip rules
         */
        bool operator==(const Oligo &);

        /**
         * Inverted comparation of oligos
         * */
        bool operator!=(const Oligo &);

        /**
         * Comparator
         */
        bool operator<(const Oligo &) const;

        /**
         * Generates list of possible nucleotydes basing on alternating
         * chip rules
         * */
        std::vector<std::string> possibilities();

        /**
         * Counts maximum overlap of oligos basing on alternating chip
         * rules. 0 is entire oligo overlap, 1 is overlap moved by 2, etc.
         * INT_MAX is returned when no overlap is possible
         * */
        int max_overlap(Oligo &);
    };

    typedef Oligo oligo;
  }
}


#endif
