// Nocturne 2023
// identification: lib/include/WAVDecoder/Decoder.hh
//

#pragma once

extern "C" {
#include "alsa/asoundlib.h"
}

#include <fstream>
#include <string>

struct ALSAHeader {
  unsigned int sample_rate;
  unsigned int nb_samples;
  unsigned short channels;
  unsigned short bits_per_sample;
  snd_pcm_format_t format;
  snd_pcm_access_t accessMethod;
};

class AbstractDecoder {
public:
  AbstractDecoder() {}

  AbstractDecoder(const AbstractDecoder &) = delete;
  AbstractDecoder(AbstractDecoder &&) = delete;
  AbstractDecoder &operator=(const AbstractDecoder &) = delete;
  AbstractDecoder &operator=(AbstractDecoder &&) = delete;

  virtual ~AbstractDecoder(){};
  const ALSAHeader &getHeader() const noexcept { return alsaHeader; }

  [[nodiscard]] virtual int getData(char *buffer, int size) = 0;
  [[nodiscard]] virtual bool getDataInterleave(char **buffer, int *size,
                                               unsigned long *frame) {
    return false;
  };
  [[nodiscard]] virtual bool getDataPlanar(char ***buffers, int *size,
                                               unsigned long *frame) {
    return false;
  };

protected:
  ALSAHeader alsaHeader{};
};
