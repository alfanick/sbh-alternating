#ifndef PUT_SBH_ALT_READER_H_
#define PUT_SBH_ALT_READER_H_

#include "spectrum.h"

#include <string>
#include <array>
#include <vector>

namespace PUT {
  namespace SBH {
    class Reader {
      public:
        Reader(const std::string& filename_);

        Chip sections;

      private:
        enum State { NONE, ODD, EVEN, OCCURENCE, OLIGO };

        State parseLine(std::string& line);

        std::string filename;
        int odd_length, even_length;

    };
  };
};

#endif
