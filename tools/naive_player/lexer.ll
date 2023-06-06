%{ /* -*- C++ -*- */
# include "utils/token.hh"
%}

%option noyywrap nounput noinput batch debug c++ yyclass="ttLexer"

integer   [0-9]+
float   [0-9]+\.[0-9]+
blank [ \t\v\f\r]

%%
{blank}+   ;
volume{blank}+{integer}  return Token::VOLUME;
speed{blank}+{float}    return Token::SPEED;
stop       return Token::STOP;
pause      return Token::PAUSE;
play       return Token::PLAY;
quit       return Token::QUIT;

.           return Token::ERROR;
%%
