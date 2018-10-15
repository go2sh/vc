#include "FormatLineParser.h"

using namespace vc;
using namespace vc::format;

namespace vc {
namespace format {
class FormatLineScopeState {
  FormatLineParser &Parser;
  std::vector<FormatLine *> *OriginalLines;
  std::unique_ptr<FormatLine> OriginalLine;

public:
  FormatLineScopeState(FormatLineParser &Parser) : Parser(Parser) {
    OriginalLines = Parser.CurrentLines;
    Parser.CurrentLines = &Parser.Line->Children;
    OriginalLine = std::move(Parser.Line);
    Parser.Line = std::make_unique<FormatLine>();
  }

  ~FormatLineScopeState() {
    Parser.CurrentLines = OriginalLines;
    Parser.Line = std::move(OriginalLine);
  }
};
} // namespace format
} // namespace vc

void FormatLineParser::parse() {
  nextToken();
  parseDesignFile();
}

void FormatLineParser::parseDesignFile() {
  do {
    switch (CurrentToken->Tok.getKind()) {
    case tok::semicolon:
      addToken(CurrentToken);
      addFormatLine();
      break;
    case tok::kw_entity:
      addToken(CurrentToken);
      parseEntityDecl();
      break;
    case tok::kw_architecture:
      addToken(CurrentToken);
      parseArchitectureDecl();
      break;
    default:
      addToken(CurrentToken);
    }
    nextToken();
  } while (!eof());
}

void FormatLineParser::parseEntityDecl() {
  assert(CurrentToken->is(tok::kw_entity));

  // Consume tokens for entity decl start
  nextToken();
  nextToken();
  nextToken();
  addFormatLine();

  if (CurrentToken->is(tok::kw_generic)) {
    // Consume port (
    nextToken();
    nextToken();
    parseInterfaceList();
    if (CurrentToken->is(tok::right_parenthesis)) {
      // Consume );
      nextToken();
      nextToken();
      addFormatLine();
    }
  }
  if (CurrentToken->is(tok::kw_generic)) {
    // Consume port (
    nextToken();
    nextToken();
    parseInterfaceList();
    if (CurrentToken->is(tok::right_parenthesis)) {
      // Consume );
      nextToken();
      nextToken();
      addFormatLine();
    }
  }
}

void FormatLineParser::parseArchitectureDecl() {

}

void FormatLineParser::parseInterfaceList() {
  ParenStack.clear();

  while (CurrentToken->isNot(tok::kw_end)) {
    switch (CurrentToken->Tok.getKind()) {
    case tok::left_parenthesis:
      ParenStack.push_back(CurrentToken);
      addToken(CurrentToken);
      break;
    case tok::right_parenthesis:
      if (!ParenStack.empty()) {
        ParenStack.pop_back();
        addToken(CurrentToken);
      } else {
        addFormatLine();
        return;
      }
      break;
    case tok::semicolon:
      addToken(CurrentToken);
      addFormatLine();
      ParenStack.clear();
      break;
    // TODO: Recovery for missed ending
    default:
      addToken(CurrentToken);
    }

    nextToken();
  }
}

void FormatLineParser::addToken(FormatToken *Token) {
  Line->Tokens.push_back(Token);
}

void FormatLineParser::addFormatLine() {
  if (Line->Tokens.empty())
    return;
  CurrentLines->push_back(Line.release());
  Line = std::make_unique<FormatLine>();
}