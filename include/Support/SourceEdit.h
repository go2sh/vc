#ifndef VC_SUPPORT_TEXTEDIT_H
#define VC_SUPPORT_TEXTEDIT_H
#include <Common/SourceLocation.h>

#include <set>
#include <string>

namespace vc {

class SourceEdit {
  SourceRange Range;
  std::string Text;

public:
  SourceEdit(SourceRange Range) : Range(Range) {}
  SourceEdit(SourceRange Range, const std::string &Text)
      : Range(Range), Text(Text) {}
  SourceEdit(SourceLocation Location, const std::string &Text)
      : Range(SourceRange(Location)), Text(Text) {}
};

class SourceEdits {
  SourceFile File;
  std::set<SourceEdit> Edits;

public:
  SourceEdits(SourceFile File) : File(File) {}

  void addEdit(const SourceEdit &Edit) { Edits.insert(Edit); }
}
} // namespace vc
#endif // !VC_SUPPORT_TEXTEDIT_H