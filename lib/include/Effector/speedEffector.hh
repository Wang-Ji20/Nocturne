#pragma once

#include "Effector/abstractEffector.hh"

class SpeedEffector: public AbstractEffector {
public:
  SpeedEffector(EffectorRef effector, double speed);
  virtual Maybe<EffectorBuf> getData() override;
  ~SpeedEffector(){};

private:
    EffectorRef effector;
    EffectorBuf buf;
    double speed;
};
