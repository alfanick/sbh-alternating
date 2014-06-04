#ifndef PUT_SBH_ALT_READER_H_
#define PUT_SBH_ALT_READER_H_

#include "spectrum.h"
#include "oligo.h"

#include <string>
#include <array>
#include <vector>

namespace PUT {
  namespace SBH {
    class Reader {
     public:
      Reader(const std::string &filename_);

      Chip sections;
      Oligo beginning;
      unsigned long long length;
      int sample_length;

     private:
      enum State { NONE, ODD, EVEN, OCCURENCE, OLIGO, INFO };

      State parseLine(std::string &line);

      std::string filename;

    };
  };
};

#endif
