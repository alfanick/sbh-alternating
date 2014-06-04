#ifndef PUT_SBH_ALT_SPECTRUM_H_
#define PUT_SBH_ALT_SPECTRUM_H_

#include "oligo.h"

#include <array>
#include <map>

namespace PUT {
  namespace SBH {

    class Spectrum : public std::map<Oligo, int> {
     public:
      void add(const std::string &o, int occurence);
    };

    typedef std::array<Spectrum, 2> Chip;
  }
}

#endif
