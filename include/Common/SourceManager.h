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

  virtual ~ContentCache();

  MemoryBuffer *getBuffer();
  std::string getPath() { return Buffer->getIdentifier(); }

  void replaceBuffer(MemoryBuffer *NewBuffer) {
    // We own the buffer so delete it, when exchagened
    if (Buffer == NewBuffer) {
      assert(0 && "Try to replace with the same buffer.");
      return;
    }
    if (Buffer) {
      delete Buffer;
    }
    Buffer = NewBuffer;
  }
};
} // namespace Detail

class SourceManager {
  std::vector<Detail::ContentCache *> FileCache;
  std::vector<SourceLocation> LocationCache;
  unsigned LastSourceLocation = 0;

public:
  SourceManager() {
    FileCache.push_back(nullptr);
    LocationCache.push_back(SourceLocation());
  };

  SourceFile createSourceFile(const std::string &Path);
  SourceFile createSourceFile(std::unique_ptr<MemoryBuffer> Buffer);

  MemoryBuffer *getBuffer(SourceFile File) const;
  std::pair<SourceFile, unsigned> getDecomposedLocation(SourceLocation);

  SourceLocation getStartOfFile(SourceFile SFile) const;

  uint32_t getColumnNumber(SourceFile File, uint32_t Offset);
  uint32_t getLineNumber(SourceFile File, uint32_t Offset);
  std::string getPath(SourceFile File);
};
} // namespace vc

#endif
