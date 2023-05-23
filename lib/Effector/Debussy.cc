#include "Effector/Debussy.hh"
#include "Effector/interleaveEffector.hh"
#include "Effector/sequenceEffector.hh"
#include "Effector/speedEffector.hh"

Debussy::Debussy(AbstractDecoder &decoder) : decoder{decoder} {
  if (decoder.getHeader().accessMethod == SND_PCM_ACCESS_RW_INTERLEAVED) {
    effector = std::make_unique<SequenceEffector>(decoder);
  } else {
    effector = std::make_unique<InterleaveEffector>(decoder);
  }
}

Maybe<EffectorBuf> Debussy::getData() { return effector->getData(); }

bool Debussy::getData(char **bufs, int *size, size_t *frame) {
  auto maybeData = effector->getData();
  if (!maybeData) {
    fprintf(stderr, "decoder finished!\n");
    return false;
  }
  auto data = maybeData.value();
  *bufs = data.data();
  *size = data.size();
  *frame = data.size() / (decoder.getHeader().bits_per_sample / 8) /
           decoder.getHeader().channels;
  return true;
}

void Debussy::speedup(double factor) {
  effector = std::make_unique<SpeedEffector>(std::move(effector), factor);
}