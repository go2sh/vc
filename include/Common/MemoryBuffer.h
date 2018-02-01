#ifndef VC_BASIC_MEMORY_BUFFER_H
#define VC_BASIC_MEMORY_BUFFER_H

#include "Common/StringRef.h"
#include <iostream>
#include <memory>
#include <string>

namespace vc {

class MemoryBuffer {
private:
  const char *BufferStart = nullptr;
  const char *BufferEnd = nullptr;

public:
  const char *getBufferStart() const { return BufferStart; }
  const char *getBufferEnd() const { return BufferEnd; }
  std::size_t getBufferSize() const { return BufferEnd - BufferStart; }

  StringRef getBuffer() const {
    return StringRef(BufferStart, getBufferSize());
  }

  void init(const char *BufferStart, const char *BufferEnd);

public:
  static std::unique_ptr<MemoryBuffer> getFile(const std::string &Filename);
};

} // namespace vc

#endif