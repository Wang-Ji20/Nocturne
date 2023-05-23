#include "Effector/interleaveEffector.hh"

InterleaveEffector::InterleaveEffector(AbstractDecoder &decoder)
    : decoder{decoder} {}

Maybe<EffectorBuf> InterleaveEffector::getData() {
  data_.clear();
  char **bufs;
  int size;
  size_t frame;
  if (!decoder.getDataPlanar(&bufs, &size, &frame)) {
    return std::nullopt;
  }
  for (size_t i = 0; i < frame; i++) {
    for (size_t j = 0; j < decoder.getHeader().channels; j++) {
      data_.insert(data_.end(), bufs[j] + i * size, bufs[j] + (i + 1) * size);
    }
  }
  return data_;
}