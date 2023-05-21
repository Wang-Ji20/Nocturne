// Nocturne 2023
// identification: tools/naive_player/main.cc

#include "ALSAInterface/ALSA.hh"
#include "Decoder/FFDecoder.hh"

#include <iostream>

void usage() { std::cerr << "Usage: naiver_player <wav file>" << std::endl; }

int main(int argc, char **argv) {
  if (argc != 2) {
    usage();
    return 1;
  }

  FFDecoder decoder(argv[1]);
  Debussy debussy(decoder);
  ALSA alsa(debussy);
  alsa.CemeteryOfInnocents();
}