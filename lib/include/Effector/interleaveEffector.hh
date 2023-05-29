#pragma once

#include "Decoder/abstractDecoder.hh"
#include "Effector/abstractEffector.hh"

class InterleaveEffector final : public AbstractEffector {
public:
  InterleaveEffector(AbstractDecoder &decoder);
  virtual bool next() override;
  virtual bool hasData() override;
  virtual EffectorBuf& getData() override;
  ~InterleaveEffector(){};

private:
  EffectorBuf data_;
  AbstractDecoder &decoder;
};