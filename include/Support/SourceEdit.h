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

  bool isInsert() const { return Text.length() > 0 && Range.length() == 0; }
  bool isReplace() const { return Text.length() > 0 && Range.length() > 0; }
  bool isDelete() const { return Text.length() == 0 && Range.length() > 0; }

  const std::string &getText() const { return Text; }
  SourceRange getRange() const { return Range; }

  friend bool operator<(const SourceEdit &LHS, const SourceEdit &RHS);
};

class SourceEdits {
  SourceFile File;
  std::set<SourceEdit> Edits;

public:
  SourceEdits(SourceFile File) : File(File) {}

  void addEdit(const SourceEdit &Edit);

  using iterator = std::set<SourceEdit>::iterator;
  using reverse_iterator = std::set<SourceEdit>::reverse_iterator;
  using const_iterator = std::set<SourceEdit>::const_iterator;
  using const_reverse_iterator = std::set<SourceEdit>::const_reverse_iterator;

  iterator begin() { return Edits.begin(); }
  iterator end() { return Edits.end(); }
  const_iterator begin() const { return Edits.begin(); }
  const_iterator end() const { return Edits.end(); }
  const_iterator cbegin() const { return Edits.cbegin(); }
  const_iterator cend() const { return Edits.cend(); }
  const_reverse_iterator rbegin() const { return Edits.rbegin(); }
  const_reverse_iterator rend() const { return Edits.rend(); }
};
} // namespace vc

#endif // !VC_SUPPORT_TEXTEDIT_H