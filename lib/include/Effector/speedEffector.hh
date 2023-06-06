#pragma once

#include "Decoder/abstractDecoder.hh"
#include "Effector/abstractEffector.hh"

class SpeedEffector : public AbstractEffector {
public:
  SpeedEffector(EffectorOwner effector, double speed, const ALSAHeader &header);
  virtual bool next() override;
  virtual bool hasData() override;
  virtual EffectorBuf& getData() override;
  void setSpeed(double speed) { this->speed = speed; };
  const ALSAHeader &getHeader() { return header; };
  ~SpeedEffector(){};

private:
  const ALSAHeader &header;
  EffectorOwner effector;
  EffectorBuf buf;
  double speed;
};
