#include "Effector/sequenceEffector.hh"

SequenceEffector::SequenceEffector(AbstractDecoder &decoder)
    : decoder{decoder} {}


Maybe<std::vector<char>> SequenceEffector::getData() {
    data_.clear();
    char *buffer;
    int size;
    size_t frame;
    if (decoder.getDataInterleave(&buffer, &size, &frame)) {
        data_.insert(data_.end(), buffer, buffer + size);
    }
    return data_;
    }