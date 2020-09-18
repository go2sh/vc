#include <cassert>
#include <memory>

#include "FormatLineParser.h"

using namespace vc;
using namespace vc::format;

namespace vc::format {

class LineLevelContext {
  FormatLineParser &Parser;

public:
  explicit LineLevelContext(FormatLineParser &Parser) : Parser(Parser) {
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
  explicit ParenthesisContext(FormatLineParser &Parser) : Parser(Parser) {
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

class ScopedLineContext {
  std::vector<FormatLine *> *OriginalLines;
  std::unique_ptr<FormatLine> OriginalLine;
  FormatLineParser &Parser;

public:
  explicit ScopedLineContext(FormatLineParser &Parser) : Parser(Parser) {
    OriginalLines = Parser.CurrentLines;
    OriginalLine = std::move(Parser.Line);
    Parser.CurrentLines = &OriginalLine->Tokens.back()->Children;
    Parser.Line = std::make_unique<FormatLine>();
  }

  ~ScopedLineContext() {
    if (!Parser.Line->Tokens.empty()) {
      Parser.addFormatLine();
    }
    Parser.Line = std::move(OriginalLine);
    Parser.CurrentLines = OriginalLines;
  }
};
}  // namespace vc::format

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
  CurrentLines->push_back(Line.release());

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
    case TokenKind::semicolon:
      nextToken();
      addFormatLine();
      break;
    case TokenKind::kw_context:
      parseContextDecl();
      break;
    case TokenKind::kw_entity:
      parseEntityDecl();
      break;
    case TokenKind::kw_architecture:
      parseArchitectureDecl();
      break;
    default:
      nextToken();
    }
  } while (!eof());

  // Push eof line
  pushToken(CurrentToken);
  addFormatLine();
}

void FormatLineParser::parseContextDecl() {
  assert(CurrentToken->is(TokenKind::kw_context));
  nextToken();

  while (CurrentToken->isNot(TokenKind::semicolon, TokenKind::kw_is)) {
    nextToken();
  }

  if (CurrentToken->is(TokenKind::kw_is)) {
    while (CurrentToken->isNot(TokenKind::kw_end)) {
      if (CurrentToken->is(TokenKind::semicolon)) {
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
    while (CurrentToken->isNot(TokenKind::kw_end)) {
      if (eof()) {
        return;
      }
      if (CurrentToken->is(TokenKind::semicolon)) {
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
  if (CurrentToken->is(TokenKind::kw_entity)) {
    nextToken();
  }
  if (CurrentToken->is(TokenKind::basic_identifier)) {
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
  } while (CurrentToken->isNot(TokenKind::kw_is));
  nextToken();
  addFormatLine();

  // Declarations
  {
    LineLevelContext LineCtx{*this};
    while (!eof() && CurrentToken->isNot(TokenKind::kw_begin)) {
      parseDeclarativeItem();
    }
  }

  // Begin
  nextToken();
  addFormatLine();

  // Statements
  {
    LineLevelContext{*this};
    while (!eof() && CurrentToken->isNot(TokenKind::kw_end)) {
      parseConcurrentStatement();
    }
  }
  nextToken();
  if (CurrentToken->is(TokenKind::kw_architecture)) {
    nextToken();
  }
  if (CurrentToken->is(TokenKind::basic_identifier)) {
    nextToken();
  }
  nextToken();
  addFormatLine();
}

void FormatLineParser::parseInterfaceList() {
  std::size_t InitialStackSize = ParenStack.size();

  while (!eof() && CurrentToken->isNot(TokenKind::kw_end)) {
    switch (CurrentToken->Tok.getKind()) {
    case TokenKind::kw_constant:
    case TokenKind::kw_signal:
    case TokenKind::kw_variable:
    case TokenKind::kw_file:
      parseObjectDeclaration();
      break;

    case TokenKind::kw_type:
      parseTypeDeclaration();
      break;

    case TokenKind::kw_pure:
    case TokenKind::kw_impure:
    case TokenKind::kw_function:
    case TokenKind::kw_procedure:
      parseSubprogram();
      break;

    case TokenKind::kw_package:
      parsePackage();

    case TokenKind::right_parenthesis:
      if (ParenStack.size() == InitialStackSize) {
        return;
      }
      break;

    case TokenKind::semicolon:
      nextToken();
      addFormatLine();
      break;
      // TODO: Recovery for missed ending

    default:
      nextToken();
    }
  }
}

void FormatLineParser::parseGenericClause() {
  if (!CurrentToken->is(TokenKind::kw_generic)) {
    return;
  }
  nextToken();

  {
    ParenthesisContext ParenCtx(*this);
    ScopedLineContext ScopeCtx(*this);
    LineLevelContext LevelCtx(*this);

    parseInterfaceList();
  }

  // Consume the ;
  nextToken();
  addFormatLine();
}

void FormatLineParser::parsePortClause() {
  if (!CurrentToken->is(TokenKind::kw_port)) {
    return;
  }
  nextToken();

  {
    ParenthesisContext ParenCtx(*this);
    ScopedLineContext ScopeCtx(*this);
    LineLevelContext LevelCtx(*this);
    parseInterfaceList();
  }

  nextToken();
  addFormatLine();
}

void FormatLineParser::parseDeclarativeItem() {
  switch (CurrentToken->Tok.getKind()) {
  case TokenKind::kw_pure:
  case TokenKind::kw_impure:
  case TokenKind::kw_function:
  case TokenKind::kw_procedure:
    parseSubprogram();
    break;
  case TokenKind::kw_package:
    parsePackage();
    break;
  case TokenKind::kw_type:
    parseTypeDeclaration();
    break;
  case TokenKind::kw_subtype:
    parseSubtypeDeclaration();
    break;
  case TokenKind::kw_component:
    parseComponentDeclaration();
    break;
  case TokenKind::kw_shared:
    nextToken();
    [[fallthrough]];
  case TokenKind::kw_variable:
  case TokenKind::kw_signal:
  case TokenKind::kw_constant:
  case TokenKind::kw_file:
    parseObjectDeclaration();
    break;
  // Fallback
  case TokenKind::semicolon:
    nextToken();
    addFormatLine();
    break;
  case TokenKind::eof:
    return;
  default:
    nextToken();
  }
}

void FormatLineParser::parseSubprogram() {
  if (CurrentToken->isAny(TokenKind::kw_pure, TokenKind::kw_impure)) {
    nextToken();
  }
  nextToken();
  nextToken();
  if (CurrentToken->isNot(TokenKind::kw_is)) {
    parseSubprogramBody();
  } else {
    // parseSubprogramInstantiation();
  }
}

void FormatLineParser::parseSubprogramBody() {
  // Genric clause
  if (CurrentToken->is(TokenKind::kw_generic)) {
    nextToken();
    nextToken();
    parseInterfaceList();
    nextToken();
  }

  // Generic map
  if (CurrentToken->is(TokenKind::kw_generic)) {
    // parseGenericMapAspect();
  }

  if (CurrentToken->is(TokenKind::kw_parameter)) {
    nextToken();
  }

  if (CurrentToken->is(TokenKind::left_parenthesis)) {
    nextToken();
    parseInterfaceList();
    nextToken();
  }

  if (CurrentToken->is(TokenKind::kw_return)) {
    nextToken();
    parseName();
  }

  // Subprogram declaration
  if (CurrentToken->is(TokenKind::semicolon)) {
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
    while (CurrentToken->isNot(TokenKind::kw_begin)) {
      parseDeclarativeItem();
    }
  }

  // Begin
  nextToken();
  addFormatLine();

  // Sequential statements
  {
    LineLevelContext LineCtx{*this};
    while (CurrentToken->isNot(TokenKind::kw_end)) {
      parseSequentialStatement();
    }
  }
  nextToken();
  if (CurrentToken->isAny(TokenKind::kw_function, TokenKind::kw_procedure)) {
    nextToken();
  }
  if (CurrentToken->is(TokenKind::basic_identifier)) {
    nextToken();
  }
  nextToken();
  addFormatLine();
}

void FormatLineParser::parsePackage() {}

void FormatLineParser::parseTypeDeclaration() {
  nextToken();
  nextToken();

  if (CurrentToken->isNot(TokenKind::kw_is)) {
    return;
  }
  nextToken();

  switch (CurrentToken->Tok.getKind()) {
  case TokenKind::kw_range:
    nextToken();
    parseExpression();
    if (CurrentToken->is(TokenKind::kw_units)) {
      parsePhysicalTypeDefinition();
    }
    break;
  case TokenKind::left_parenthesis:
    while (!eof() && CurrentToken->isNot(TokenKind::right_parenthesis)) {
      nextToken();
    }
    nextToken();
    break;
  case TokenKind::kw_array:
    parseArrayTypeDefinition();
    break;
  case TokenKind::kw_record:
    parseRecordTypeDefinition();
  }
}

void FormatLineParser::parseSubtypeDeclaration() {}

void FormatLineParser::parsePhysicalTypeDefinition() {}

void FormatLineParser::parseArrayTypeDefinition() {}

void FormatLineParser::parseRecordTypeDefinition() {}

void FormatLineParser::parseObjectDeclaration() {
  if (CurrentToken->isAny(TokenKind::kw_constant, TokenKind::kw_signal, TokenKind::kw_variable,
                          TokenKind::kw_file)) {
    nextToken();
  }

  // Identifier list
  nextToken();
  while (!eof() && CurrentToken->is(TokenKind::comma)) {
    nextToken();
    nextToken();
  }

  // Colon
  nextToken();

  // Direction
  if (CurrentToken->isAny(TokenKind::kw_in, TokenKind::kw_out, TokenKind::kw_inout,
                          TokenKind::kw_buffer, TokenKind::kw_linkage)) {
    nextToken();
  }

  parseSubtypeIndication();

  // Signal mode or file mode
  if (CurrentToken->isAny(TokenKind::kw_register, TokenKind::kw_bus)) {
    nextToken();
  } else if (CurrentToken->is(TokenKind::kw_open)) {
    parseExpression();
  }

  // File name or default value
  if (CurrentToken->isAny(TokenKind::variable_assignment, TokenKind::kw_is)) {
    parseExpression();
  }
}

void FormatLineParser::parseConcurrentStatement() {
  while (!eof() && CurrentToken->isNot(TokenKind::semicolon)) {
    if (CurrentToken->is(TokenKind::kw_end)) {
      return;
    }
    nextToken();
  }
  nextToken();
  addFormatLine();
}

void FormatLineParser::parseSequentialStatement() {
  while (!eof() && CurrentToken->isNot(TokenKind::semicolon)) {
    if (CurrentToken->is(TokenKind::kw_end)) {
      return;
    }
    nextToken();
  }
  nextToken();
  addFormatLine();
}

void FormatLineParser::parseName() {
  while (CurrentToken->isAny(TokenKind::basic_identifier, TokenKind::extended_identifier,
                             TokenKind::dot, TokenKind::tick, TokenKind::left_parenthesis)) {
    if (CurrentToken->isNot(TokenKind::left_parenthesis)) {
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
    case TokenKind::basic_identifier:
    case TokenKind::extended_identifier:
      IsLogicalUnary = false;
      parseName();
      break;

    case TokenKind::based_literal:
    case TokenKind::bit_string_literal:
    case TokenKind::character_literal:
    case TokenKind::decimal_literal:
    case TokenKind::string_literal:
      IsLogicalUnary = false;
      nextToken();
      break;

    case TokenKind::plus:
    case TokenKind::minus:
      PrecLevel = prec::Sign;
      nextToken();

    case TokenKind::kw_not:
    case TokenKind::kw_abs:
      IsLogicalUnary = true;
      PrecLevel = prec::Miscellaneous;
      nextToken();
      break;
    case TokenKind::kw_and:
    case TokenKind::kw_nand:
    case TokenKind::kw_or:
    case TokenKind::kw_nor:
    case TokenKind::kw_xor:
    case TokenKind::kw_xnor:
      if (!IsLogicalUnary) {
        prec::Level Prec =
            getBinaryOperatorPrecedence(CurrentToken->Tok.getKind());
        if (Prec > PrecLevel) {
        }
        if (Prec < PrecLevel) {
        }
      } else {
      }

    case TokenKind::left_parenthesis: {
      ParenthesisContext ParenCtx(*this);
      prec::Level OutsidePrecLevel = PrecLevel;
      PrecLevel = prec::Unknown;
      parseExpression();
      PrecLevel = OutsidePrecLevel;
      break;
    }

    case TokenKind::kw_to:
    case TokenKind::kw_downto:
      // Handle range expression
      PrecLevel = prec::Unknown;
      IsLogicalUnary = true;
      nextToken();
      break;

    case TokenKind::right_parenthesis:
    case TokenKind::semicolon:
    case TokenKind::kw_then:
    case TokenKind::kw_loop:
    case TokenKind::kw_generate:
      // Stop parsing on unmatched parenthesis, semicolon and some keywords
      if (CurrentToken->Previous) {
        CurrentToken->Previous->FakeRParen;
      }
      return;

    default:
      nextToken();
    }
  }
}

void FormatLineParser::parseListRangeIndex() {
  while (!eof() && CurrentToken->isNot(TokenKind::right_parenthesis)) {
    parseExpression();
    switch (CurrentToken->Tok.getKind()) {
    case TokenKind::arrow:
    case TokenKind::comma:
      nextToken();
    case TokenKind::kw_downto:
    case TokenKind::kw_to:
      nextToken();
      parseExpression();
      break;
    default:
      return;
    }
  }
}

void FormatLineParser::parseSubtypeIndication() {
  auto parseResolutionIndication = [this](auto &self) -> void {
    if (ParenStack.size() > 50) {
      throw false; // TODO;
    }

    // Consume single element function
    if (CurrentToken->isAny(TokenKind::basic_identifier, TokenKind::extended_identifier)) {
      parseName();
    }

    // Array or record resolution
    if (CurrentToken->is(TokenKind::left_parenthesis)) {
      ParenthesisContext ParenCtx(*this);
      bool Valid = true;

      while (!eof() && CurrentToken->isNot(TokenKind::right_parenthesis)) {
        if (CurrentToken->isAny(TokenKind::basic_identifier,
                                TokenKind::extended_identifier)) {
          parseName();
        }
        self(self);
        if (CurrentToken->is(TokenKind::comma)) {
          nextToken();
        }
      }
    }
  };

  parseResolutionIndication(parseResolutionIndication);

  // Try to parse the type mark
  if (CurrentToken->isAny(TokenKind::basic_identifier, TokenKind::extended_identifier)) {
    parseName();
  }

  if (CurrentToken->is(TokenKind::kw_range)) {
    nextToken();
    parseExpression();
  } else if (CurrentToken->is(TokenKind::left_parenthesis)) {
    ParenthesisContext Paren(*this);
    do {
      parseExpression();
      if (CurrentToken->is(TokenKind::comma)) {
        nextToken();
      }
    } while (CurrentToken->isNot(TokenKind::right_parenthesis, TokenKind::semicolon));
  }
}

void FormatLineParser::parseComponentDeclaration() {
  nextToken();
  nextToken();
  if (CurrentToken->is(TokenKind::kw_is)) {
    nextToken();
  }
  addFormatLine();

  {
    LineLevelContext LineCtx(*this);
    parseGenericClause();
    parsePortClause();
  }
  while (!eof() && CurrentToken->isNot(TokenKind::kw_end)) {
    nextToken();
  }
  nextToken();
  nextToken();
  if (CurrentToken->isAny(TokenKind::basic_identifier, TokenKind::extended_identifier)) {
    nextToken();
  }
}