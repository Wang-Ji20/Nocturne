#pragma once
#include <any>

#ifndef __FLEX_LEXER_H
#include <iostream>
class yyFlexLexer {
public:
    yyFlexLexer(std::istream*) {};
};
#endif


enum Token {
    PLAY,
    PAUSE,
    STOP,
    VOLUME,
    ERROR,
    QUIT,
};

class ttLexer: public yyFlexLexer {
public:
    ttLexer(std::istream* in): yyFlexLexer(in) {};
    int yylex();
};