#include "oligo.h"
#include "sequence.h"

using namespace PUT::SBH;

Oligo::Oligo() : length(0), sequence("") {}

Oligo::Oligo(Sequence seq) {
  set_sequence(seq);
}

Oligo::Oligo(const Oligo& another) {
  set_sequence(another.sequence);
}

void Oligo::set_sequence(Sequence seq) {
  std::transform(seq.begin(), seq.end(), seq.begin(), ::tolower);
  sequence = seq;
  length = seq.size();
  if (seq.find_first_of(Nucleotide::X) == std::string::npos)
    type = CLASSIC;
  else
    type = SPECIAL;
}

Oligo& Oligo::operator=(Sequence sequence) {
  set_sequence(sequence);
  return *this;
}

bool Oligo::operator==(const Oligo & another) {
  if(another.length != length)
    return false;

  for(int i = 0; i < sequence.size(); ++i)
    if(another.sequence[i] != Nucleotide::X && sequence[i] != Nucleotide::X && sequence[i] != another.sequence[i])
      return false;

  return true;
}

bool Oligo::operator!=(const Oligo & another) {
  return !operator==(another);
}

bool Oligo::operator<(const Oligo & another) const {
  return sequence < another.sequence;
}

std::vector<Sequence> Oligo::possibilities() {
  std::vector<Sequence> res;
  std::queue<Sequence> queue;
  queue.push(sequence);
  while (!queue.empty()) {
    Sequence current = queue.front();
    queue.pop();
    int pos = current.find_first_of(Nucleotide::X);
    if(pos != std::string::npos) {
      std::string letters = "acgt";
      for(int i = 0; i < letters.size(); ++i) {
        Sequence copy(current);
        copy[pos] = letters[i];
        queue.push(copy);
      }
    }
    else
      res.push_back(current);
  }
  return res;
}

int Oligo::max_overlap(Oligo & another) {
  if(another.length != length)
    throw std::invalid_argument("Overlapping oligos should be the same length");
  for(int offset = 0; offset < length-1; offset+=2) {
    if(sequence.compare(offset, std::string::npos, another.sequence, 0, another.length - offset) == 0)
      return offset/2;
  }
  return INT_MAX;
}
