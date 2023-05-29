#pragma once

#include "Decoder/FFDecoder.hh"
#include "Effector/abstractEffector.hh"
#include <array>

// read in data from decoder
class SequenceEffector final : public AbstractEffector {
public:
  // constructor for INTERLEAVED data
  SequenceEffector(AbstractDecoder &decoder);
  virtual bool next() override;
  virtual bool hasData() override;
  virtual EffectorBuf& getData() override;
  ~SequenceEffector(){};

private:
  EffectorBuf data_;
  AbstractDecoder &decoder;
};