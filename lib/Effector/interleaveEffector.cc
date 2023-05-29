#include "Effector/interleaveEffector.hh"

InterleaveEffector::InterleaveEffector(AbstractDecoder &decoder)
    : decoder{decoder} {}

bool InterleaveEffector::next() {
  data_.clear();
  char **bufs;
  int size;
  size_t frame;
  if (!decoder.getDataPlanar(&bufs, &size, &frame)) {
    DRAIN;
  }
  for (size_t i = 0; i < frame; i++) {
    for (size_t j = 0; j < decoder.getHeader().channels; j++) {
      data_.insert(data_.end(), bufs[j] + i * size, bufs[j] + (i + 1) * size);
    }
  }
  return true;
}

bool InterleaveEffector::hasData() {
  DRAINRET;
  return true;
}

EffectorBuf& InterleaveEffector::getData() {
  return data_;
}