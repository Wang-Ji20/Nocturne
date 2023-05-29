#pragma once

#include "utils/ruskell.hh"

#include <memory>
#include <vector>

#define DRAINRET   \
do {               \
  if (drain) {     \
    return false;  \
  }                \
} while(0)

#define DRAIN      \
do {               \
  drain = true;    \
  return false;    \
} while(0)

using EffectorBuf = std::vector<char>;

class AbstractEffector {
public:
  virtual bool next() = 0;
  virtual bool hasData() = 0;
  virtual EffectorBuf& getData() = 0;
  virtual ~AbstractEffector(){};

protected:
  [[maybe_unused]] bool drain {false};
};

using EffectorOwner = std::unique_ptr<AbstractEffector>;