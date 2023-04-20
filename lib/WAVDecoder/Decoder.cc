// Nocturne 2023
// identification: lib/WAVDecoder/Decoder.cc

#include "WAVDecoder/Decoder.hh"
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


static void seekDataSection(int fd) {
  // automaton
  // 0: d
  // 1: da
  // 2: dat
  // 3: data, return
  // read one byte at a time
  // if we see "data", return
  enum { INIT, D, DA, DAT } automaton = INIT;

  char c;
  while (read(fd, &c, 1) == 1) {
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
        return;
      } else {
        automaton = INIT;
      }
      break;
    }
  }
  throw std::runtime_error("UNREACHABLE: Failed to find data section");
}

Decoder::Decoder(const char *filename) {
  fd = open(filename, O_RDONLY);
  if (fd == -1) {
    throw std::runtime_error(strerror(fd));
  }
  if (read(fd, &header, sizeof(WAVHeader)) != sizeof(WAVHeader)) {
    throw std::runtime_error("Failed to read WAV header");
  }
  wavChecker(header);
  seekDataSection(fd);
}

int Decoder::getData(char *buffer, int size) {
  return read(fd, buffer, size);
}