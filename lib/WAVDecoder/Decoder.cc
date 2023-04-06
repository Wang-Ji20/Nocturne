// Nocturne 2023
// identification: lib/WAVDecoder/Decoder.cc

#include "WAVDecoder/Decoder.hh"
#include <fcntl.h>
#include <cstring>
#include <stdexcept>

// sanity check
static void wavChecker(const WAVHeader& header) {
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

Decoder::Decoder(const char* filename) {
  fd = open(filename, O_RDONLY);
  if (fd == -1) {
    throw std::runtime_error(strerror(fd));
  }
  if (read(fd, &header, sizeof(WAVHeader)) != sizeof(WAVHeader)) {
    throw std::runtime_error("Failed to read WAV header");
  }
  wavChecker(header);
}
