#include "spectrum.h"
#include <iostream>
namespace PUT {
namespace SBH {

void Spectrum::add(const std::string& oligo, int occurence) {
  insert(std::make_pair(Oligo(oligo), occurence));
}

}
}
