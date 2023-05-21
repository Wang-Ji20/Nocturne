// Nocturne 2023
// identification: tools/naive_player/main.cc

#include "ALSAInterface/ALSA.hh"
#include "Decoder/WAVDecoder.hh"

#include <iostream>

void usage() { std::cerr << "Usage: naiver_player <wav file>" << std::endl; }

int main(int argc, char **argv) {
  if (argc != 2) {
    usage();
    return 1;
  }

  WAVDecoder decoder(argv[1]);
  ALSA alsa(decoder, true);
  alsa.naivePlay();
}