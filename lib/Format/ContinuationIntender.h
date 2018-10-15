#ifndef VC_FORMAT_CONTINUATION_INTENDER_H
#include "FormatLineParser.h"
#include "FormatToken.h"

namespace vc {
namespace format {
struct LineState {
  FormatToken *PreviousToken;
  FormatToken *NextToken;
  FormatLine *Line;

  /// The column index on the current line
  unsigned Column;
};

class ContinuationIntender {
  
}
} // namespace format
} // namespace vc
#endif // !VC_FORMAT_CONTINUATION_INTENDER_H