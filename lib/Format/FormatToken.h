#ifndef VC_FORMAT_FORMATTOKEN_H
#define VC_FORMAT_FORMATTOKEN_H
#include <string_view>
#include <vector>

#include <Common/SourceLocation.h>
#include <Parse/Token.h>

namespace vc::format {

struct FormatLine;

/**
 * Contains the format information
 */
struct FormatToken {
  FormatToken() = default;

  /** Token to format */
  Token Tok;

  /** Next token in line */
  FormatToken *Next = nullptr;

  /** Previous token in line */
  FormatToken *Previous = nullptr;

  /** Matching FormatToken, if this token is a parenthesis */
  FormatToken *MatchingParenthesis = nullptr;

  /** Vector of FormatLines connected to this token
   *
   * This is expecially used for interface lists and type
   * definitions.
   */
  std::vector<FormatLine *> Children;

  /** Text of the token whithout trailing whitespaces */
  std::string_view TokenText;

  /** Range of whitespace characters before this token */
  SourceRange WhitespaceRange;

  /** The whitespace text before this token. */
  std::string_view WhitespacePrefix;

  /** Force a linebreak before this Token */
  bool MustBreakBefore = false;

  /** Can break before this token */
  bool CanBreakBefore = true;

  /** New lines before current Token */
  unsigned NewLinesBefore = 0;

  /** Spaces required before this token */
  unsigned SpacesRequiredBefore = 0;

  /** Number of parenthesis around the token */
  unsigned NestingLevel = 0;

  /** Number of fake left parenthesis inserted from operator predecence */
  unsigned FakeLParen = 0;

  /** Number of fake right parenthesis */
  unsigned FakeRParen = 0;

  /** Number of columns needed to display this token */
  unsigned ColumnWidth = 0;

  [[nodiscard]] auto isFirstOnLine() const -> bool {
    return Previous == nullptr;
  }

  [[nodiscard]] auto is(TokenKind Kind) -> bool { return Tok.is(Kind); }
  template <typename... T>
  [[nodiscard]] auto isNot(TokenKind K1, T... K) -> bool {
    return Tok.isNot(K1, K...);
  };

  [[nodiscard]] auto isNot(TokenKind Kind) -> bool { return Tok.isNot(Kind); }
  template <typename... Ts>[[nodiscard]] auto isAny(Ts... Args) -> bool {
    return Tok.isAny(Args...);
  }
};
}  // namespace vc::format
#endif
