#pragma once
#include "FlexLexer.h"
#include "utils/token.hh"

class Lexer {
public:
  Lexer(std::istream *in) : lexer{in} {};

  Token next() { return token = (Token)lexer.yylex(); }

  std::any getValue() {
    const char *c = lexer.YYText();
    fprintf(stderr, "%s\n", c);
    switch (token) {
    case VOLUME:
      for (; *c && !std::isdigit(c[0]); c++)
        ;
      value = strtol(c, nullptr, 10);
      return value;
    case SPEED:
      for (; *c && !std::isdigit(c[0]); c++)
        ;
      value = strtod(c, nullptr);
      return value;
    default:
      return std::any();
    }
  }

private:
  ttLexer lexer;
  Token token;
  std::any value;
};
