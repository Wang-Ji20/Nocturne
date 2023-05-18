// Nocturne 2023
// identification: lib/include/WAVDecoder/Decoder.hh
//

#pragma once
#include <fstream>
#include <string>

struct ALSAHeader {
  unsigned int sample_rate;
  unsigned short channels;
  unsigned short bits_per_sample;
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

protected:
  ALSAHeader alsaHeader{};
};
