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
  ContentCache() : Buffer(nullptr) {}
  ContentCache(const std::string &Path) : Buffer(nullptr), Path(Path) {}
  MemoryBuffer *getBuffer();
  std::string getPath() { return Path;}

private:
  void setBuffer(MemoryBuffer *Buf) { Buffer = Buf; }
  void replaceBuffer(MemoryBuffer *Buf) {
    if (Buffer) {
      delete Buffer;
    }

    Buffer = Buf;
  }
};
} // namespace Detail

class SourceManager {
  std::vector<Detail::ContentCache *> FileCache;

public:
  SourceManager() { FileCache.push_back(nullptr); };

  SourceFile createSourceFile(const std::string &Path);

  MemoryBuffer *getBuffer(SourceFile File);
  std::pair<SourceFile, unsigned> getDecomposedLocation(SourceLocation);

  uint32_t getColumnNumber(SourceFile File, uint32_t Offset);
  uint32_t getLineNumber(SourceFile File, uint32_t Offset);
  std::string getPath(SourceFile File);
};
} // namespace vc

#endif
