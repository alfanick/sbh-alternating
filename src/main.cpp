#include <iostream>

#include "reader.h"
#include "oligo.h"
#include <vector>
#include <assert.h>

using namespace PUT::SBH;

int main() {
  PUT::SBH::Reader* r = new PUT::SBH::Reader("data/example.seq");

  std::cout << "Hello World!\n";

  oligo o1("axcxt");
  assert(o1.type = Oligo::SPECIAL);
  
  std::vector<std::string> list = o1.possibilities();
  std::cout << "Possible oligos for " << o1.sequence << ":\n";
  for(auto iter = list.begin(); iter != list.end(); ++iter)
    std::cout << "\t" << *iter << "\n";

  oligo o2("xtcgt");
  assert(o1 == o2);

  o2 = "cgtca";
  assert(o1 != o2);
  assert(o2.type == Oligo::CLASSIC);

  return 0;
}
