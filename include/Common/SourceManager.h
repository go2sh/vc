#ifndef VC_BASIC_SOURCE_MANAGER_H
#define VC_BASIC_SOURCE_MANAGER_H

#include "Common/MemoryBuffer.h"
#include "Common/SourceLocation.h"
#include <vector>

namespace vc {
namespace Detail {
class ContentCache {
  MemoryBuffer *Buffer;
  std::string Path;

public:
  ContentCache(const std::string &Path) : Buffer(nullptr), Path(Path) {}
  MemoryBuffer *getBuffer();

private:
  void setBuffer(MemoryBuffer *Buf) { Buffer = Buf; }
};
} // namespace Detail

class SourceManager {
  std::vector<Detail::ContentCache *> FileCache;

public:
  SourceManager() { FileCache.push_back(nullptr); };

  SourceFile createSourceFile(const std::string &Path);

  MemoryBuffer *getBuffer(SourceFile File) { }
  uint32_t getLineNumber(SourceFile File, uint32_t Offset);
};
} // namespace vc

#endif
