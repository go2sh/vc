#ifndef VC_FORMAT_FORMAT_H
#define VC_FORMAT_FORMAT_H

#include <Common/SourceLocation.h>

namespace vc {
namespace format {
struct FormatStyle {

  enum UseTabsStyle { UTS_Never, UTS_ForIdention };
  UseTabsStyle IndentionChar;

  unsigned TabSize;
  unsigned IndentionWidth;

  unsigned ColumnLimit;
};

void reformat(const SourceManager &SrcMgr, SourceFile SF, const FormatStyle &Style);
} // namespace format
} // namespace vc

#endif // !VC_FORMAT_FORMAT_H
