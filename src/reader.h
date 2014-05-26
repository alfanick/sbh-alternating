#ifndef PUT_SBH_ALT_READER_H_
#define PUT_SBH_ALT_READER_H_

#include <string>

namespace PUT {
  namespace SBH {
    class Reader {
      private:
        enum State { NONE, ODD, EVEN, OCCURENCE, OLIGO };

      public:
        Reader(const std::string& filename_);

      private:
        State parseLine(const std::string& line);

        std::string filename;
    };
  };
};

#endif
