#include "FormatLineParser.h"

using namespace vc;
using namespace vc::format;

namespace vc {
namespace format {

class LineLevelContext {
  FormatLineParser &Parser;

public:
  LineLevelContext(FormatLineParser &Parser) : Parser(Parser) {
    Parser.Level++;
  }
  ~LineLevelContext() { Parser.Level--; }
};

/**
 * Parenthesis Context RAII-Class
 *
 * This class consumes parenthesis and manages the nesting level.
 */
class ParenthesisContext {
  FormatLineParser &Parser;

public:
  ParenthesisContext(FormatLineParser &Parser) : Parser(Parser) {
    FormatToken *LParen = Parser.CurrentToken;

    // First add the token to the line, then update the stack
    Parser.nextToken();
    Parser.ParenStack.push_back(LParen);
  }

  ~ParenthesisContext() {
    FormatToken *LParen = Parser.ParenStack.back();
    LParen->MatchingParenthesis = Parser.CurrentToken;
    Parser.CurrentToken->MatchingParenthesis = LParen;
    // First remove the LParen from the stack,
    // then add the token to the line
    Parser.ParenStack.pop_back();
    Parser.nextToken();
  }
};
} // namespace format
} // namespace vc

void FormatLineParser::readToken() { CurrentToken = *TokenIt++; }

void FormatLineParser::nextToken() {
  if (eof()) {
    return;
  }

  pushToken(CurrentToken);
  readToken();
}

void FormatLineParser::pushToken(FormatToken *Token) {
  if (!Line->Tokens.empty()) {
    FormatToken *Previous = Line->Tokens.back();
    Token->Previous = Previous;
    Previous->Next = Token;
  }

  Token->NestingLevel = ParenStack.size();

  if (MustBreakBeforeNextToken) {
    Token->MustBreakBefore = true;
  }
  MustBreakBeforeNextToken = false;

  Line->Tokens.push_back(Token);
}

void FormatLineParser::addFormatLine() {
  if (Line->Tokens.empty())
    return;
  Line->Level = Level;
  Lines.push_back(Line.release());

  Line = std::make_unique<FormatLine>();
}

void FormatLineParser::parse() {
  TokenIt = Tokens.cbegin();
  readToken();
  parseDesignFile();
}

void FormatLineParser::parseDesignFile() {
  do {
    switch (CurrentToken->Tok.getKind()) {
    case tok::semicolon:
      nextToken();
      addFormatLine();
      break;
    case tok::kw_context:
      parseContextDecl();
      break;
    case tok::kw_entity:
      parseEntityDecl();
      break;
    case tok::kw_architecture:
      parseArchitectureDecl();
      break;
    default:
      nextToken();
    }
    nextToken();
  } while (!eof());

  // Push eof line
  pushToken(CurrentToken);
  addFormatLine();
}

void FormatLineParser::parseContextDecl() {
  assert(CurrentToken->is(tok::kw_context));
  nextToken();

  while (CurrentToken->isNot(tok::semicolon, tok::kw_is)) {
    nextToken();
  }

  if (CurrentToken->is(tok::kw_is)) {
    while (CurrentToken->isNot(tok::kw_end)) {
      if (CurrentToken->is(tok::semicolon)) {
        nextToken();
        addFormatLine();
      } else {
        nextToken();
      }
    }
  }
  addFormatLine();
}

void FormatLineParser::parseEntityDecl() {

  // Consume tokens for entity decl start
  nextToken();
  nextToken();
  nextToken();
  addFormatLine();

  {
    LineLevelContext LineCtx(*this);
    parseGenericClause();
    parsePortClause();

    // Parse the declarations
    while (CurrentToken->isNot(tok::kw_end)) {
      if (eof()) {
        return;
      }
      if (CurrentToken->is(tok::semicolon)) {
        nextToken();
        addFormatLine();
      } else {
        // TODO: Body declarations
        // TODO: entity statements
        nextToken();
      }
    }
  }

  // Parse end
  nextToken();
  if (CurrentToken->is(tok::kw_entity)) {
    nextToken();
  }
  if (CurrentToken->is(tok::basic_identifier)) {
    nextToken();
  }
  nextToken();
  addFormatLine();
}

void FormatLineParser::parseArchitectureDecl() {
  // archtecture .. is
  nextToken();
  nextToken();
  nextToken();
  do {
    // TODO: Parse name
    nextToken();
  } while (CurrentToken->isNot(tok::kw_is));
  nextToken();

  // Declarations
  {
    LineLevelContext LineCtx{*this};
    while (CurrentToken->isNot(tok::kw_begin)) {
      parseBlockDeclarativeItem();
    }
  }

  // Begin
  nextToken();

  // Statements
  {
    LineLevelContext{*this};
    while (CurrentToken->isNot(tok::kw_end)) {
      parseConcurrentStatement();
    }
  }
  nextToken();
  if (CurrentToken->is(tok::kw_architecture)) {
    nextToken();
  }
  if (CurrentToken->is(tok::basic_identifier)) {
    nextToken();
  }
  nextToken();
}

void FormatLineParser::parseInterfaceList() {
  ParenStack.clear();

  while (CurrentToken->isNot(tok::kw_end)) {
    if (eof()) {
      return;
    }

    // TODO: Add more elements (function, include type, package)
    // TODO: add interface object parser
    switch (CurrentToken->Tok.getKind()) {
    case tok::left_parenthesis:
      ParenStack.push_back(CurrentToken);
      nextToken();
      break;
    case tok::right_parenthesis:
      if (!ParenStack.empty()) {
        ParenStack.pop_back();
        nextToken();
      } else {
        addFormatLine();
        return;
      }
      break;
    case tok::semicolon:
      nextToken();
      addFormatLine();
      ParenStack.clear();
      break;
    // TODO: Recovery for missed ending
    default:
      nextToken();
    }
  }
}

void FormatLineParser::parseGenericClause() {
  if (CurrentToken->is(tok::kw_generic)) {
    // Consume generic (
    nextToken();
    nextToken();
    addFormatLine();

    {
      LineLevelContext LineCtx(*this);
      parseInterfaceList();
    }

    if (CurrentToken->is(tok::right_parenthesis)) {
      // Consume );
      nextToken();
      nextToken();
      addFormatLine();
    }
  }
}

void FormatLineParser::parsePortClause() {
  if (CurrentToken->is(tok::kw_port)) {
    // Consume port (
    nextToken();
    nextToken();
    addFormatLine();

    {
      LineLevelContext LineCtx(*this);
      parseInterfaceList();
    }

    if (CurrentToken->is(tok::right_parenthesis)) {
      // Consume );
      nextToken();
      nextToken();
      addFormatLine();
    }
  }
}

void FormatLineParser::parseBlockDeclarativeItem() {
  switch (CurrentToken->Tok.getKind()) {
  case tok::kw_pure:
  case tok::kw_impure:
  case tok::kw_function:
  case tok::kw_procedure:
    parseSubprogram();
    break;
  case tok::kw_package:
    parsePackage();
    break;
  case tok::kw_type:
    parseTypeDeclaration();
    break;
  case tok::kw_subtype:
    parseSubtypeDeclaration();
    break;
  }
}

void FormatLineParser::parseSubprogram() {
  if (CurrentToken->isAny(tok::kw_pure, tok::kw_impure)) {
    nextToken();
  }
  nextToken();
  nextToken();
  if (CurrentToken->isNot(tok::kw_is)) {
    parseSubprogramBody();
  } else {
    // parseSubprogramInstantiation();
  }
}

void FormatLineParser::parseSubprogramBody() {
  // Genric clause
  if (CurrentToken->is(tok::kw_generic)) {
    nextToken();
    nextToken();
    parseInterfaceList();
    nextToken();
  }

  // Generic map
  if (CurrentToken->is(tok::kw_generic)) {
    // parseGenericMapAspect();
  }

  if (CurrentToken->is(tok::kw_parameter)) {
    nextToken();
  }

  if (CurrentToken->is(tok::left_parenthesis)) {
    nextToken();
    parseInterfaceList();
    nextToken();
  }

  if (CurrentToken->is(tok::kw_return)) {
    nextToken();
    parseName();
  }

  // Subprogram declaration
  if (CurrentToken->is(tok::semicolon)) {
    nextToken();
    addFormatLine();
    return;
  }

  // Subprogram specification
  nextToken();
  addFormatLine();

  // Declarations
  {
    LineLevelContext LineCtx{*this};
    while (CurrentToken->isNot(tok::kw_begin)) {
      parseBlockDeclarativeItem(); // TODO: Custom
    }
  }

  // Begin
  nextToken();
  addFormatLine();

  // Sequential statements
  {
    LineLevelContext LineCtx{*this};
    while (CurrentToken->isNot(tok::kw_end)) {
      parseSequentialStatement();
    }
  }
  nextToken();
  if (CurrentToken->isAny(tok::kw_function, tok::kw_procedure)) {
    nextToken();
  }
  if (CurrentToken->is(tok::basic_identifier)) {
    nextToken();
  }
  nextToken();
  addFormatLine();
}

void FormatLineParser::parsePackage() {}

void FormatLineParser::parseTypeDeclaration() {}

void FormatLineParser::parseSubtypeDeclaration() {}

void FormatLineParser::parseConcurrentStatement() {}

void FormatLineParser::parseSequentialStatement() {}

void FormatLineParser::parseName() {
  while (CurrentToken->isAny(tok::basic_identifier, tok::extended_identifier,
                             tok::dot, tok::tick, tok::left_parenthesis)) {
    if (CurrentToken->isNot(tok::left_parenthesis)) {
      nextToken();
    } else {
      ParenthesisContext ParenCtx(*this);
      parseListRangeIndex();
    }
  }
}

void FormatLineParser::parseExpression() {
  std::vector<FormatToken *> PrecStack;
  FormatToken *PrecBegin = CurrentToken;
  prec::Level PrecLevel = prec::Unknown;
  bool IsLogicalUnary = true;

  PrecStack.push_back(PrecBegin);

  while (!eof()) {
    switch (CurrentToken->Tok.getKind()) {
    case tok::basic_identifier:
    case tok::extended_identifier:
      IsLogicalUnary = false;
      parseName();
      break;

    case tok::based_literal:
    case tok::bit_string_literal:
    case tok::character_literal:
    case tok::decimal_literal:
    case tok::string_literal:
      IsLogicalUnary = false;
      nextToken();
      break;

    case tok::plus:
    case tok::minus:
      PrecLevel = prec::Sign;
      nextToken();

    case tok::kw_not:
    case tok::kw_abs:
      IsLogicalUnary = true;
      PrecLevel = prec::Miscellaneous;
      nextToken();
      break;
    case tok::kw_and:
    case tok::kw_nand:
    case tok::kw_or:
    case tok::kw_nor:
    case tok::kw_xor:
    case tok::kw_xnor:
      if (!IsLogicalUnary) {
        prec::Level Prec =
            getBinaryOperatorPrecedence(CurrentToken->Tok.getKind());
        if (Prec > PrecLevel) {
        }
        if (Prec < PrecLevel) {
          if (!Line->Tokens.empty()) {
            FormatToken *End = Line->Tokens.back();
            End->ExpressionLevel--;
          }
        }

      } else {
      }

    case tok::left_parenthesis: {
      ParenthesisContext ParenCtx(*this);
      parseExpression();
      break;
    }

    case tok::right_parenthesis:
    case tok::semicolon:
    case tok::kw_then:
    case tok::kw_loop:
    case tok::kw_generate:
    case tok::kw_to:
    case tok::kw_downto:
      // Stop parsing on unmatched parenthesis, semicolon and some keywords
      return;

    default:
      nextToken();
    }
  }
}

void FormatLineParser::parseListRangeIndex() {
  while (CurrentToken->isNot(tok::right_parenthesis)) {
    if (eof()) {
      return;
    }

    parseExpression();
    switch (CurrentToken->Tok.getKind()) {
    case tok::arrow:
    case tok::comma:
      nextToken();
    case tok::kw_downto:
    case tok::kw_to:
      nextToken();
      parseExpression();
      break;
    default:
      return;
    }
  }
}