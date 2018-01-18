#ifndef VC_BASIC_SOURCE_MANAGER_H
#define VC_BASIC_SOURCE_MANAGER_H

#include "Common/SourceLocation.h"
#include <vector>

namespace vc {
namespace SourceManager {
class ContentCache {
  MemoryBuffer *Buffer;

public:
  MemoryBuffer *getBuffer() const { return Buffer; }
};
} // namespace SourceManager

using namespace vc::SourceManager;
class SourceManager {
  std::vector<ContentCache *> FileCache;

public:
  SourceManager() { FileCache.push_back(nullptr); };
};
} // namespace vc

#endif
