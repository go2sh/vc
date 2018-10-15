#include <Support/SourceEdit.h>

using namespace vc;

namespace vc {
bool operator<(const SourceEdit &LHS, const SourceEdit &RHS) {
  return LHS.Range.getBegin() < RHS.Range.getBegin();
}
} // namespace vc

void SourceEdits::addEdit(const SourceEdit &Edit) {
  Edits.insert(Edit);
}