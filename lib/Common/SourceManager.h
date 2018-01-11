#include "Common/MemoryBuffer.h"

namespace vc {
namespace SourceManager {
class ContentCache {
  MemoryBuffer *Buffer;

public:
  MemoryBuffer *getBuffer() const { return Buffer; }
}
} // namespace SourceManager
} // namespace vc