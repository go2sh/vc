#ifndef VC_FORMAT_FORMATTOKEN_H
#define VC_FORMAT_FORMATTOKEN_H
#include <Parse/Token.h>
#include <Common/SourceLocation.h>

namespace vc {
namespace format {
struct FormatToken {
  FormatToken() {}

  /** Token to format */
  Token Tok;

  /** Next token in line */
  FormatToken *NextToken = nullptr;

  /** Previous token in line */
  FormatToken *PreviousToken = nullptr;

  /** Matching FormatToken, if this token is a parenthesis */
  FormatToken *MatchingParenthesis = nullptr;

  /** Text of the token whithout trailing whitespaces */
  StringRef TokenText;

  /** Level of nesting */
  unsigned NestingLevel = 0;

  /** Idention level */
  unsigned IndentionLevel = 0;

  /** Force a linebreak before this Token */
  bool MustBreakBefore = false;

  /** New lines before current Token */
  unsigned NewLinesBefore = 0;

  /** Range of whitespace characters before this token */
  SourceRange WhitespaceRange;

  /** Column of the first char of this Token */
  unsigned Column = 0;
};
} // namespace format
} // namespace vc
#endif
