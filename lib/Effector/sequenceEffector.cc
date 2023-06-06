#include "Effector/sequenceEffector.hh"

SequenceEffector::SequenceEffector(AbstractDecoder &decoder)
    : decoder{decoder} {}

bool SequenceEffector::next() {
  DRAINRET;
  data_.clear();
  char *buffer;
  int size;
  size_t frame;
  if (decoder.getDataInterleave(&buffer, &size, &frame)) {
    data_.insert(data_.end(), buffer, buffer + size);
    return true;
  }
  DRAIN;
}

bool SequenceEffector::hasData() {
  DRAINRET;
  return true;
}

EffectorBuf &SequenceEffector::getData() { return data_; }