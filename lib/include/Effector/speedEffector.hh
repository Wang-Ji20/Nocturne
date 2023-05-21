#pragma once

#include "Effector/abstractEffector.hh"

class SpeedEffector final : public AbstractEffector {
public:
  SpeedEffector(AbstractEffector &effector, double speed);
  virtual Maybe<std::vector<char>> getData() override;
  ~SpeedEffector(){};

private:
    AbstractEffector &effector;
    double speed;
};
