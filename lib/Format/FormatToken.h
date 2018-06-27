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

  /** Level of nesting */
  unsigned NestingLevel = 0;

  /** Idention level */
  unsigned IndentionLevel = 0;

  /** Force a linebreak before this Token */
  bool MustBreakBefore = false;

  /** Number of spaces before this token
   * 
   * This value does not account for idention as this is handled in IndentionLevel.
   */
  unsigned SpacesBefore = 0;

  /** Range of whitespace characters before this token */
  SourceRange WhitespaceRange;
};
} // namespace format
} // namespace vc
#endif
