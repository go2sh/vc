#ifndef VC_FORMAT_EDIT_MANAGER_H
#define VC_FORMAT_EDIT_MANAGER_H
#include <string>

#include <Common/SourceLocation.h>
#include <Format/Format.h>
#include <Support/SourceEdit.h>

#include "FormatToken.h"

namespace vc {
namespace format {

class EditManager {
  SourceEdits Edits;
  const FormatStyle &Style;

public:
  EditManager(SourceFile SF, const FormatStyle &Style)
      : Edits(SF), Style(Style) {}

  void replaceWhitespace(const FormatToken &FT, unsigned NewLines,
                         unsigned Spaces);

  SourceEdits &getSourceEdits() { return Edits; }

private:
  std::string createWhitespace(unsigned NewLines, unsigned Spaces);
};

} // namespace format
} // namespace vc
#endif // !VC_FORMAT_EDIT_MANAGER_H