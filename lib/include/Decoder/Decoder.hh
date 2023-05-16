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

class Decoder {
public:
  Decoder() {}

  Decoder(const Decoder &) = delete;
  Decoder(Decoder &&) = delete;
  Decoder &operator=(const Decoder &) = delete;
  Decoder &operator=(Decoder &&) = delete;

  ~Decoder() { file.close(); }
  const ALSAHeader &getHeader() const noexcept { return alsaHeader; }

  [[nodiscard]] virtual int
  getData(char *buffer, int size) = 0;

protected:
  ALSAHeader alsaHeader{};
  std::fstream file{};
  size_t data_offset{};
};
