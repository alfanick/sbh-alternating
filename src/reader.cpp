#include "reader.h"

#include <fstream>
#include <iostream>
namespace PUT {
namespace SBH {

Reader::Reader(const std::string& filename_) : filename(filename_) {
  std::ifstream input(filename_);

  for (std::string line; std::getline(input, line);) {
    switch (parseLine(line)) {
      case State::NONE:
        break;
      case State::ODD:
        break;
      case State::EVEN:
        break;
      case State::OCCURENCE:
        break;
      case State::OLIGO:
        break;
    }

  }

  input.close();
}

Reader::State Reader::parseLine(const std::string& line) {
  return Reader::State::NONE;
}

}
}
