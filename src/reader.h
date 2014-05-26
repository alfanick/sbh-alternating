#ifndef PUT_SBH_ALT_READER_H_
#define PUT_SBH_ALT_READER_H_

#include <string>

namespace PUT {
  namespace SBH {
    class Reader {
      public:
        Reader(const std::string& filename_);

      private:
        std::string filename;
    };
  };
};

#endif
