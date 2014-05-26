#include "reader.h"

#include <fstream>
#include <iostream>
namespace PUT {
namespace SBH {

Reader::Reader(const std::string& filename_) : filename(filename_) {
  std::ifstream input(filename_);

  int occurence = -1;
  std::vector<std::string> &current_section = sections[0];

  for (std::string line; std::getline(input, line);) {
    switch (parseLine(line)) {
      case State::ODD:
        std::cerr << "ODD\n";
        current_section = sections[0];
        odd_length = std::stoi(line);
        break;
      case State::EVEN:
        std::cerr << "EVEN\n";
        even_length = std::stoi(line);
        break;
      case State::OCCURENCE:
        occurence = line[0] == 'N' ? -1 : std::stoi(line);
        std::cerr << "OCC " << occurence << "\n";
        break;
      case State::OLIGO:
        std::cerr << "OLIGO " << line << "\n";
        current_section.push_back(line);
        break;
      case State::NONE:
        break;
    }

  }

  input.close();
}

Reader::State Reader::parseLine(std::string& line) {
  if (line.find(";ALTERNATING-O") == 0) {
    line.erase(0, 15);
    return Reader::State::ODD;
  } else if (line.find(";ALTERNATING-E") == 0) {
    line.erase(0, 15);
    return Reader::State::EVEN;
  } else if (line[0] == '>') {
    line.erase(0, 1);
    return Reader::State::OCCURENCE;
  } else if (line[0] == 'A' || line[0] == 'T' || line[0] == 'G' || line[0] == 'C') {
    return Reader::State::OLIGO;
  }
  return Reader::State::NONE;
}

}
}
