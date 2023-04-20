// Nocturne 2023
// identification: tools/naive_player/main.cc

#include "ALSAInterface/ALSA.hh"

#include <iostream>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <wav file>" << std::endl;
    return 1;
  }

  Decoder decoder(argv[1]);
  ALSA alsa(decoder);
  alsa.play();
}