#pragma once

#include "utils/ruskell.hh"
#include <memory>
#include <vector>

#include "Decoder/abstractDecoder.hh"
#include "Effector/abstractEffector.hh"

// this class extracts data from decoder, and then transform them
// to a nice format for ALSA to play.
// It can also be used to perform speedup/slowdown of the audio.
class Debussy {
public:
  Debussy(AbstractDecoder &decoder);
  Maybe<EffectorBuf> getData();

  // for compatibility with ALSA
  [[nodiscard]] bool getData(char **buffer, int *size, size_t *frame);
  const ALSAHeader &getHeader() const noexcept { return decoder.getHeader(); }
  void speedup(double factor);

private:
  // the effector that is currently being used as root
  std::unique_ptr<AbstractEffector> effector;
  AbstractDecoder &decoder;
};