#ifndef VC_FORMAT_FORMATTOKEN_H
#define VC_FORMAT_FORMATTOKEN_H
#include <Common/SourceLocation.h>
#include <Parse/Token.h>

namespace vc {
namespace format {
struct FormatToken {
  FormatToken() {}

  /** Token to format */
  Token Tok;

  /** Next token in line */
  FormatToken *Next = nullptr;

  /** Previous token in line */
  FormatToken *Previous = nullptr;

  /** Matching FormatToken, if this token is a parenthesis */
  FormatToken *MatchingParenthesis = nullptr;

  /** Text of the token whithout trailing whitespaces */
  StringRef TokenText;

  /** Force a linebreak before this Token */
  bool MustBreakBefore = false;

  /** Can break before this token */
  bool CanBreakBefore = true;

  /** New lines before current Token */
  unsigned NewLinesBefore = 0;

  /** Spaces required before this token */
  unsigned SpacesRequiredBefore = 0;

  /** Range of whitespace characters before this token */
  SourceRange WhitespaceRange;

  /** The whitespace text before this token. */
  StringRef WhitespacePrefix;

  /** Expression Level */
  int ExpressionLevel = 0;

  unsigned NestingLevel = 0;

  /** Number of columns needed to display this token */
  unsigned ColumnWidth = 0;

  bool isFirstOnLine() const { return Previous == nullptr; }

  bool is(tok::TokenKind Kind) { return Tok.is(Kind); }
  bool isNot(tok::TokenKind Kind) { return Tok.isNot(Kind); }
  template <typename... T> bool isNot(tok::TokenKind K1, T... K) {
    return Tok.isNot(K1, K...);
  };
  template <typename... Ts> bool isAny(Ts... Args) {
    return Tok.isAny(Args...);
  }
};
} // namespace format
} // namespace vc
#endif
