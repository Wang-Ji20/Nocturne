#include "Effector/Debussy.hh"
#include "Effector/interleaveEffector.hh"
#include "Effector/sequenceEffector.hh"
#include "Effector/speedEffector.hh"

Debussy::Debussy(AbstractDecoder &decoder) : decoder{decoder} {
  std::unique_ptr<AbstractEffector> sourceEffector;
  if (decoder.getHeader().accessMethod == SND_PCM_ACCESS_RW_INTERLEAVED) {
    sourceEffector = std::make_unique<SequenceEffector>(decoder);
  } else {
    sourceEffector = std::make_unique<InterleaveEffector>(decoder);
  }
  effector = std::make_unique<SpeedEffector>(std::move(sourceEffector), speed,
                                             decoder.getHeader());
}

EffectorBuf &Debussy::getData() { return effector->getData(); }

bool Debussy::next() {
  if (drain) {
    return false;
  }
  if (!effector->next()) {
    drain = true;
    return false;
  }
  return true;
}

void Debussy::getData(char **bufs, int *size, size_t *frame) {
  auto &data = effector->getData();
  *bufs = data.data();
  *size = data.size();
  *frame = data.size() / (decoder.getHeader().bits_per_sample / 8) /
           decoder.getHeader().channels / speed;
}

void Debussy::speedup(double factor) {
  speed *= factor;
  effector->setSpeed(speed);
}