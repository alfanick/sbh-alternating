#include "sequencer.h"

using namespace PUT::SBH;

Sequencer::Sequencer() {};

Sequencer::Sequencer(Chip & chip, int n, int k) {
  feed(chip, n, k);
}

void Sequencer::feed(Chip & chip, int n, int k) {
  this->chip = chip;
  this->n = n;
  this->k = k;
  results.clear();
}
