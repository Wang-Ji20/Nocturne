// Nocturne 2023
// identification: lib/include/WAVDecoder/Decoder.hh
//

#pragma once
#include <string>
#include <unistd.h>

struct WAVHeader {
  char riff[4];          // "RIFF"
  unsigned int size;     // size of file - 8
  char wave[4];          // "WAVE"
  char fmt[4];           // "fmt "
  unsigned int fmt_size; // size of fmt chunk
  unsigned short format;
  unsigned short channels;
  unsigned int sample_rate;
  unsigned int byte_rate;     // sample_rate * channels * bits_per_sample / 8
  unsigned short block_align; // channels * bits_per_sample / 8
  unsigned short bits_per_sample;
} __attribute__((packed));

class Decoder {
public:
  Decoder(const char *filename);

  Decoder(const Decoder &) = delete;
  Decoder(Decoder &&) = delete;
  Decoder &operator=(const Decoder &) = delete;
  Decoder &operator=(Decoder &&) = delete;

  ~Decoder() { close(fd); };
  const WAVHeader &getHeader() const { return header; };

  int getData(char *buffer, int size);

private:
  WAVHeader header{};
  int fd{};
};
