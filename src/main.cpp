#include <iostream>

#include <unistd.h>
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

  Reader r(filepath);

  std::cout << "Beginning from reader: " << r.beginning.sequence << "\n";

  Sequencer sequencer(r.sections, r.length, r.sample_length, r.beginning.sequence);

  sequencer.run();

  sleep(1000);

  return 0;
}
