#pragma once

#include "Effector/speedEffector.hh"
#include "libavfilter/avfilter.h"

class AtempoEffector : public SpeedEffector {
public:
  AtempoEffector(EffectorRef effector, double speed);
  virtual Maybe<EffectorBuf> getData() override;
  ~AtempoEffector(){};

private:
  EffectorRef effector;
  EffectorBuf buf;
  double speed;
};