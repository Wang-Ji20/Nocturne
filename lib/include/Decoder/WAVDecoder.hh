#pragma once
#include "Decoder/abstractDecoder.hh"

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

class WAVDecoder : public AbstractDecoder {
public:
  WAVDecoder(std::string_view filename);
  int getData(char *buffer, int size) override;
  const WAVHeader &getWAVHeader() const noexcept { return header; }
  ~WAVDecoder() { file.close(); }

private:
  WAVHeader header{};
  std::fstream file;
};
