// Nocturne 2023
// identification: tools/naive_player/main.cc

#include "ALSAInterface/ALSA.hh"
#include "Decoder/FFDecoder.hh"
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
  return std::make_unique<FFDecoder>(s);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    usage();
    return 1;
  }

  launch();

  DecoderRef decoder = getDecoder(argv[1]);
  Debussy debussy(*decoder);
  ALSA alsa(debussy);
  alsa.Lachaise();

  while (prompt(), std::cin) {
    Lexer lexer(&std::cin);
    Token token = lexer.next();
    switch (token) {
    case Token::VOLUME:
      alsa.setVolume(std::any_cast<long>(lexer.getValue()));
      break;
    case Token::SPEED:
      debussy.speedup(std::any_cast<double>(lexer.getValue()));
      break;
    case Token::PAUSE:
      alsa.pause();
      break;
    case Token::PLAY:
      alsa.resume();
      break;
    case Token::QUIT:
      return 0;
    default:
      std::cerr << "Unknown command" << std::endl;
    }
  }
}