// Nocturne 2023
// identification: tools/naive_player/main.cc

#include "ALSAInterface/ALSA.hh"
#include "Decoder/FLACDecoder.hh"
#include "Decoder/WAVDecoder.hh"
#include "nlexer.hpp"
#include "utils/color.hh"

#include <iostream>
#include <memory>
#include <thread>

using DecoderRef = std::unique_ptr<AbstractDecoder>;

void usage() { std::cerr << "Usage: naive_player <wav file>" << std::endl; }

// launching ascii art
void launch() {
  std::cout << "Welcome to Nocturne 2023\n"
            << "---------------------------------\n"
            << "  __  __  __  __  __  __\n"
            << " /  \\/  \\/  \\/  \\/  \\/  \\\n"
            << " \\__/\\__/\\__/\\__/\\__/\\__/\n"
            << " /  \\/  \\/  \\/  \\/  \\/  \\\n"
            << " \\__/\\__/\\__/\\__/\\__/\\__/\n"
            << " /  \\/  \\/  \\/  \\/  \\/  \\\n"
            << " \\__/\\__/\\__/\\__/\\__/\\__/\n"
            << " /  \\/  \\/  \\/  \\/  \\/  \\\n"
            << " \\__/\\__/\\__/\\__/\\__/\\__/\n"
            << " /  \\/  \\/  \\/  \\/  \\/  \\\n"
            << " \\__/\\__/\\__/\\__/\\__/\\__/\n"
            << "---------------------------------" << std::endl;
}

void prompt() {
  fputs(ANSI_COLOR_CYAN "% " ANSI_RESET_ALL, stdout);
  fflush(stdout);
}

DecoderRef getDecoder(char *filename) {
  std::string_view s(filename);
  // ends with wav
  if (s.size() >= 4 && s.substr(s.size() - 4) == ".wav") {
    return std::make_unique<WAVDecoder>(s);
  }
  // ends with flac
  if (s.size() >= 5 && s.substr(s.size() - 5) == ".flac") {
    return std::make_unique<FLACDecoder>(s);
  }
  throw std::runtime_error("Unknown file format");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    usage();
    return 1;
  }

  launch();

  DecoderRef decoder = getDecoder(argv[1]);
  ALSA alsa(*decoder, false, 5512);
  alsa.play();

  while (prompt(), std::cin) {
    Lexer lexer(&std::cin);
    Token token = lexer.next();
    switch (token) {
    case Token::VOLUME:
      alsa.setVolume(std::any_cast<long>(lexer.getValue()));
      break;
    case Token::PAUSE:
      alsa.pause();
      break;
    case Token::PLAY:
      alsa.play();
      break;
    case Token::QUIT:
      return 0;
    default:
      std::cerr << "Unknown command" << std::endl;
    }
  }
}