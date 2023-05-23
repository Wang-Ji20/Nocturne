#pragma once

#include "utils/ruskell.hh"

#include <memory>
#include <vector>

using EffectorBuf = std::vector<char>;

class AbstractEffector {
public:
  virtual Maybe<EffectorBuf> getData() = 0;
  virtual ~AbstractEffector(){};

private:
};

using EffectorRef = std::unique_ptr<AbstractEffector>;