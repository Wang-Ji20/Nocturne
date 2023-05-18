// Nocturne 2023
// identification: lib/WAVDecoder/Decoder.cc

#include "Decoder/WAVDecoder.hh"
#include <cstring>
#include <fcntl.h>
#include <stdexcept>

// sanity check
static void wavChecker(const WAVHeader &header) {
  // do some sanity checks like FFmpeg does
  if (strncmp(header.riff, "RIFF", 4) != 0) {
    throw std::runtime_error("Not a RIFF file");
  }
  if (strncmp(header.wave, "WAVE", 4) != 0) {
    throw std::runtime_error("Not a WAVE file");
  }
  if (strncmp(header.fmt, "fmt ", 4) != 0) {
    throw std::runtime_error("Not a fmt file");
  }
}

static int seekDataSection(std::fstream &file) {
  // automaton
  // 0: d
  // 1: da
  // 2: dat
  // 3: data, return
  // read one byte at a time
  // if we see "data", return
  enum { INIT, D, DA, DAT } automaton = INIT;

  char c;
  while (file.get(c)) {
    switch (automaton) {
    case INIT:
      if (c == 'd') {
        automaton = D;
      }
      break;
    case D:
      if (c == 'a') {
        automaton = DA;
      } else {
        automaton = INIT;
      }
      break;
    case DA:
      if (c == 't') {
        automaton = DAT;
      } else {
        automaton = INIT;
      }
      break;
    case DAT:
      if (c == 'a') {
        return file.tellg();
      } else {
        automaton = INIT;
      }
      break;
    }
  }
  throw std::runtime_error("UNREACHABLE: Failed to find data section");
}

WAVDecoder::WAVDecoder(std::string_view filename) {
  file.open(filename.data(), std::ios::in | std::ios::binary);
  if (!file.is_open()) {
    file.clear();
    throw std::runtime_error("Failed to open WAV file");
  }
  file.read((char *)&header, sizeof(WAVHeader));
  if (file.bad() || file.gcount() != sizeof(WAVHeader)) {
    file.clear();
    throw std::runtime_error("Failed to read WAV header");
  }
  wavChecker(header);
  alsaHeader = ALSAHeader{
      .sample_rate = header.sample_rate,
      .channels = header.channels,
      .bits_per_sample = header.bits_per_sample,
  };
  seekDataSection(file);
}

[[nodiscard]] auto WAVDecoder::getData(char *buffer, int size) -> int {
  file.read(buffer, size);
  return file.gcount();
}