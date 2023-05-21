#pragma once

#include "Decoder/abstractDecoder.hh"
#include "Effector/abstractEffector.hh"

class InterleaveEffector final : public AbstractEffector {
public:
  InterleaveEffector(AbstractDecoder &decoder);
  virtual Maybe<std::vector<char>> getData() override;
  ~InterleaveEffector(){};

private:
  std::vector<char> data_;
  AbstractDecoder &decoder;
};