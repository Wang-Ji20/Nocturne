#include "Effector/Debussy.hh"
#include "Effector/interleaveEffector.hh"
#include "Effector/sequenceEffector.hh"

Debussy::Debussy(AbstractDecoder &decoder) : decoder{decoder} {
  if (decoder.getHeader().accessMethod == SND_PCM_ACCESS_RW_INTERLEAVED) {
    effector = std::make_unique<SequenceEffector>(decoder);
  } else {
    effector = std::make_unique<InterleaveEffector>(decoder);
  }
}

Maybe<std::vector<char>> Debussy::getData() { return effector->getData(); }

bool Debussy::getData(char **bufs, int *size, size_t *frame) {
  auto maybeData = effector->getData();
  if (!maybeData) {
    return false;
  }
  auto data = maybeData.value();
  *bufs = data.data();
  *size = data.size();
  *frame = data.size() / (decoder.getHeader().bits_per_sample / 8) /
           decoder.getHeader().channels;
  return true;
}