#ifndef VC_PARSER_H
#define VC_PARSER_H
#include "Lexer/Lexer.h"

namespace vc {
class Parser {
  Parser(Lexer &Lexer) : Lexer(Lexer) {
  };

private:
    Lexer Lexer;

public:
  void ParseDesignFile();
};
}; // namespace vc
#endif