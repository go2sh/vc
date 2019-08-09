#ifndef VC_FORMAT_FORMATLINEPARSER_H
#define VC_FORMAT_FORMATLINEPARSER_H

#include "FormatToken.h"
#include <Common/OperatorPrecedence.h>
#include <Format/Format.h>

#include <memory>
#include <stack>
#include <vector>

namespace vc {
namespace format {

struct FormatLine {
  /// List of FormatTokens
  std::vector<FormatToken *> Tokens;
  /// Indention Level
  unsigned Level = 0;
  bool isFirst;
};

class FormatLineParser {
  const FormatStyle &Style;

  const std::vector<FormatToken *> &Tokens;
  FormatToken *CurrentToken;
  std::unique_ptr<FormatLine> Line;
  std::vector<FormatLine *> Lines;

  unsigned Level = 0;

  bool MustBreakBeforeNextToken = false;

  std::vector<FormatToken *>::const_iterator TokenIt;
  /// Parenthesis stack for parsing constructs with conflicting
  /// parentheses definitions like interfaces or instantiation
  std::vector<FormatToken *> ParenStack;

public:
  FormatLineParser(const FormatStyle &Style, std::vector<FormatToken *> &Tokens)
      : Style(Style), Tokens(Tokens) {
    Line = std::make_unique<FormatLine>();
    Line->isFirst = true;
  }

  ~FormatLineParser() {
    for (auto Line : Lines) {
      std::free(Line);
    }
  }

  friend class LineLevelContext;
  friend class ParenthesisContext;

  std::vector<FormatLine *> &getLines() { return Lines; }

  void parse();

private:
  void readToken();
  void nextToken();
  void pushToken(FormatToken *Token);

  void addFormatLine();
  bool eof() { return CurrentToken->is(tok::eof); }

  void parseDesignFile();
  void parseDesignUnit();

  void parseContextDecl();
  void parseEntityDecl();
  void parseArchitectureDecl();

  void parseInterfaceList();
  void parseGenericClause();
  void parsePortClause();

  /* Items */
  void parseBlockDeclarativeItem();

  void parseSubprogram();
  void parseSubprogramBody();
  void parseSubprogramInstantiation();

  void parsePackage();
  void parsePackageDeclaration();
  void parsePackageBody();
  void parsePackageInstantiation();

  void parseTypeDeclaration();

  void parseSubtypeDeclaration();
  void parseConstantDeclaration();
  void parseSignalDeclaration();
  void parseVariableDeclaration();
  void parseFileDeclaration();
  void parseAliasDeclaration();
  void parseComponentDeclaration();
  void parseAttributeDeclaration();
  void parseAttributeSpecification();
  void parseConfigurationSpecification();
  // TODO: Disconnect
  void parseUseClause();
  // TODO: Group
  // TODO: PSL

  /* Concurrent Statements */
  void parseConcurrentStatement();

  /* Sequential Statements */
  void parseSequentialStatement();

  /* Expr */
  void parseName();
  void parseExpression();
  void parseListRangeIndex();
};
} // namespace format
} // namespace vc

#endif // !VC_FORMAT_FORMATLINEPARSER_H
