#include "reader.h"

#include <fstream>
#include <iostream>
namespace PUT {
namespace SBH {

Reader::Reader(const std::string& filename_) : filename(filename_) {
  std::ifstream input(filename_);

  for (std::string line; std::getline(input, line);) {
    std::cout << line << std::endl;
  }
}

}
}
