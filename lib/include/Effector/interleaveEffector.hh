#pragma once

#include "Decoder/abstractDecoder.hh"
#include "Effector/abstractEffector.hh"

class InterleaveEffector final : public AbstractEffector {
public:
  InterleaveEffector(AbstractDecoder &decoder);
  virtual Maybe<EffectorBuf> getData() override;
  ~InterleaveEffector(){};

private:
  EffectorBuf data_;
  AbstractDecoder &decoder;
};