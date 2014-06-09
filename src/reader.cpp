#include "reader.h"

#include <fstream>
#include <iostream>
#include <climits>

namespace PUT {
  namespace SBH {
    Reader::Reader() : filename("stdin"){
      read(std::cin);
    }

    Reader::Reader(std::string filename_) : filename(filename_) {
      std::ifstream input(filename_);
      read(input);
    }

    void Reader::read(std::istream &input) {
      int occurence = INT_MAX;
      int current_section = 0;
      int lpos = -1;

      for (std::string line; std::getline(input, line);) {
        switch (parseLine(line)) {
          case State::INFO:
            lpos = line.find("|");
            beginning = line.substr(0, lpos);
            length = std::stoull(line.substr(lpos+1, line.length() - lpos));
            break;

          case State::ODD:
            current_section = 0;
            sample_length = std::stoi(line);
            break;

          case State::EVEN:
            current_section = 1;
            sample_length = std::stoi(line);
            break;

          case State::OCCURENCE:
            occurence = line[0] == 'N' ? INT_MAX : std::stoi(line);
            break;

          case State::OLIGO:
            sections[current_section].add(line, occurence);
            break;

          case State::NONE:
            break;
        }

      }
    }

    Reader::State Reader::parseLine(std::string &line) {
      if (line[0] == 'A' || line[0] == 'T' || line[0] == 'G' || line[0] == 'C') {
        return Reader::State::OLIGO;
      } else if (line[0] == '>') {
        line.erase(0, 1);
        return Reader::State::OCCURENCE;
      } else if (line.find(";INFO") == 0) {
        line.erase(0, 6);
        return Reader::State::INFO;
      } if (line.find(";ALTERNATING-O") == 0) {

        line.erase(0, 15);
        return Reader::State::ODD;
      } else if (line.find(";ALTERNATING-E") == 0) {
        line.erase(0, 15);
        return Reader::State::EVEN;
      }

      return Reader::State::NONE;
    }

  }
}

