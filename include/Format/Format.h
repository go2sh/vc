#ifndef VC_FORMAT_FORMAT_H
#define VC_FORMAT_FORMAT_H

#include <Common/SourceLocation.h>

namespace vc {
namespace format {
struct FormatStyle {

  enum struct UseTabsStyle { Never, ForIdention };
  UseTabsStyle UseTabs = UseTabsStyle::Never;

  unsigned TabSize = 4;
  unsigned IndentionWidth = 2;

  unsigned ColumnLimit = 80;

  unsigned MaxEmptyLines = 1;
};

void reformat(const SourceManager &SrcMgr, SourceFile SF, const FormatStyle &Style);
} // namespace format
} // namespace vc

#endif // !VC_FORMAT_FORMAT_H
