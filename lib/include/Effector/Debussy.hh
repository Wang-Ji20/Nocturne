#pragma once

#include "utils/ruskell.hh"
#include <memory>
#include <vector>

#include "Decoder/abstractDecoder.hh"
#include "Effector/speedEffector.hh"

// this class extracts data from decoder, and then transform them
// to a nice format for ALSA to play.
// It can also be used to perform speedup/slowdown of the audio.
// build a default architecture: speedEffector -- interleaveEffector or sequenceEffector
class Debussy {
public:
  Debussy(AbstractDecoder &decoder);
  [[nodiscard]] bool next();
  [[nodiscard]] bool hasData() {return !drain; };
  EffectorBuf& getData();

  // for compatibility with ALSA
  void getData(char **buffer, int *size, size_t *frame);
  const ALSAHeader &getHeader() const noexcept { return decoder.getHeader(); }
  void speedup(double factor);

private:
  // the effector that is currently being used as root
  std::unique_ptr<SpeedEffector> effector;
  AbstractDecoder &decoder;
  double speed {1.0};
  bool drain {false};
};