#include <iostream>

#include "reader.h"
#include "oligo.h"
#include "sequencer.h"
#include <vector>
#include <assert.h>

using namespace PUT::SBH;

int main(int argc, char **argv) {
  std::string filepath = "data/example.seq";

  if(argc > 1) 
    filepath = argv[1];

  PUT::SBH::Reader* r = new PUT::SBH::Reader(filepath);

  std::cerr << "Odd: \n";
  for (auto& l : r->sections[0]) {
    std::cerr << l.first.sequence << " * " << l.second << std::endl;
  }

  std::cout << "Beginning from reader: " << r->beginning.sequence << "\n";

  Sequencer sequencer(r->sections, r->length, r->sample_length, r->beginning.sequence);
  sequencer.run();

  return 0;
}
