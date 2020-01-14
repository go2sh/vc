#ifndef VC_FORMAT_FORMAT_H
#define VC_FORMAT_FORMAT_H

#include <Common/SourceLocation.h>

namespace vc {
namespace format {
struct FormatStyle {

  /** Tab usage */
  enum struct UseTabsStyle {
    /** Use tabs never */
    Never,
    /** Use tabs for indention */
    ForIdention
  };

  /** Setting, when to use tabs */
  UseTabsStyle UseTabs = UseTabsStyle::Never;

  /** Size of a tabulator */
  unsigned TabSize = 4;

  /** Indention width */
  unsigned IndentionWidth = 2;

  /** Column limit */
  unsigned ColumnLimit = 80;

  /** Number of empty lines between two lines */
  unsigned MaxEmptyLines = 1;

  /*
   * Extended
   */
  /** Closing brackets behaviour */
  enum class ClosingBracketsStyle {
    /** Put bracket on the same line */
    NoNewline,
    /** Put bracket on a new line */
    NewlineIndent,
    /** Put bracket on a new line and dedent */
    NewlineDedent
  };

  /** Placement of closing brackets */
  ClosingBracketsStyle ClosingBrackets = ClosingBracketsStyle::NewlineDedent;
};

void reformat(const SourceManager &SrcMgr, SourceFile SF,
              const FormatStyle &Style);
} // namespace format
} // namespace vc

#endif // !VC_FORMAT_FORMAT_H
