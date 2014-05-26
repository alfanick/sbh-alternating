#include <iostream>

#include "reader.h"

int main() {
  PUT::SBH::Reader* r = new PUT::SBH::Reader("data/example.seq");

  std::cout << "Hello World!\n";

  return 0;
}
