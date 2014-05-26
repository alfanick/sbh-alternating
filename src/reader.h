#ifndef PUT_SBH_ALT_READER_H_
#define PUT_SBH_ALT_READER_H_

#include <string>
#include <array>
#include <vector>

namespace PUT {
  namespace SBH {
    class Reader {
      private:
        enum State { NONE, ODD, EVEN, OCCURENCE, OLIGO };

      public:
        Reader(const std::string& filename_);

        std::array<std::vector<std::string>, 2> sections;

      private:
        State parseLine(std::string& line);

        std::string filename;
        int odd_length, even_length;

    };
  };
};

#endif
