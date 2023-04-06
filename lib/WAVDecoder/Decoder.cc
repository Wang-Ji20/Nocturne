// Nocturne 2023
// identification: lib/WAVDecoder/Decoder.cc

#include "WAVDecoder/Decoder.hh"
#include <fcntl.h>
#include <cstring>
#include <stdexcept>

// sanity check
static bool wavChecker(const WAVHeader& header) {
  // do some sanity checks like FFmpeg does
  if (strncmp(header.riff, "RIFF", 4) != 0) {
    return false;
  }
  if (strncmp(header.wave, "WAVE", 4) != 0) {
    return false;
  }
  if (strncmp(header.fmt, "fmt ", 4) != 0) {
    return false;
  }
  if (strncmp(header.data, "data", 4) != 0) {
    return false;
  }
  // FFmpeg do this so we do too
  if (header.channels != 1 && header.channels != 2) {
    return false;
  }
  return true;
}

Decoder::Decoder(const char* filename) {
  fd = open(filename, O_RDONLY);
  if (fd == -1) {
    throw std::runtime_error(strerror(fd));
  }
  if (read(fd, &header, sizeof(WAVHeader)) != sizeof(WAVHeader)) {
    throw std::runtime_error("Failed to read WAV header");
  }
  if (!wavChecker(header)) {
    throw std::runtime_error("Invalid WAV file");
  }
}
