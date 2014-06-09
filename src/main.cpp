#include <iostream>

#include <unistd.h>
#include "reader.h"
#include "oligo.h"
#include "sequencer.h"
#include <vector>
#include <assert.h>

using namespace PUT::SBH;

int main(int argc, char** argv) {
  Reader *r;

  if (argc > 1)
    r = new Reader(argv[1]);
  else
    r = new Reader();

  std::cerr << "Beginning from reader: " << r->beginning.sequence << "\n";

  Sequencer sequencer(r->sections, r->length, r->sample_length, r->beginning.sequence);

  sequencer.run();

  return 0;
}
