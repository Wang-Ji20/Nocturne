#pragma once

#include "utils/ruskell.hh"

#include <vector>

class AbstractEffector {
public:
  virtual Maybe<std::vector<char>> getData() = 0;
  virtual ~AbstractEffector(){};

private:
};
