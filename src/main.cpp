#include <iostream>

#include "reader.h"
#include "oligo.h"
#include <vector>
#include <assert.h>

using namespace PUT::SBH;

int main() {
  PUT::SBH::Reader* r = new PUT::SBH::Reader("data/example.seq");

  std::cerr << "Odd: \n";
  for (auto& l : r->sections[0]) {
    std::cerr << l.first.sequence << " * " << l.second << std::endl;
  }

  std::cerr << "Even: \n";
  for (auto& l : r->sections[1]) {
    std::cerr << l.first.sequence << " * " << l.second << std::endl;
  }


  oligo o1("axcxt");
  assert(o1.type == Oligo::SPECIAL);

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
