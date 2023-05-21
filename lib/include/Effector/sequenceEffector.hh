#pragma once

#include "Decoder/FFDecoder.hh"
#include "Effector/abstractEffector.hh"
#include <array>

// read in data from decoder
class SequenceEffector final : public AbstractEffector {
public:
  // constructor for INTERLEAVED data
  SequenceEffector(AbstractDecoder &decoder);
  virtual Maybe<std::vector<char>> getData() override;
  ~SequenceEffector(){};

private:
  std::vector<char> data_;
  AbstractDecoder &decoder;
};