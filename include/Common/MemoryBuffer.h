#ifndef VC_BASIC_MEMORY_BUFFER_H
#define VC_BASIC_MEMORY_BUFFER_H

#include "Common/StringRef.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>

namespace vc {

class MemoryBuffer {
  const char *BufferStart = nullptr;
  const char *BufferEnd = nullptr;

public:
  virtual ~MemoryBuffer();

  const char *getStart() const { return BufferStart; }
  const char *getEnd() const { return BufferEnd; }
  std::size_t getSize() const { return BufferEnd - BufferStart; }

  virtual StringRef getIdentifier() { return "<invalid buffer>"; }

  StringRef getBuffer() const { return StringRef(BufferStart, getSize()); }

  friend bool operator==(const MemoryBuffer &LHS, const MemoryBuffer &RHS) {
    return std::equal(LHS.getStart(), LHS.getEnd(), RHS.getStart(),
                      RHS.getEnd());
  }

protected:
  void init(const char *BufferStart, const char *BufferEnd);

public:
  static std::unique_ptr<MemoryBuffer>
  getMemoryBuffer(StringRef Data, const std::string &Filename);
  static std::unique_ptr<MemoryBuffer> getFile(const std::string &Filename);
  static std::unique_ptr<MemoryBuffer> getSTDIN();
};

class DefaultMemoryBuffer : public MemoryBuffer {

public:
  using MemoryBuffer::getEnd;
  using MemoryBuffer::getSize;
  using MemoryBuffer::getStart;

  char *getStart() { return const_cast<char *>(MemoryBuffer::getStart()); }
  char *getEnd() { return const_cast<char *>(MemoryBuffer::getEnd()); }

  virtual StringRef getIdentifier() {
    return StringRef(reinterpret_cast<const char *>(this + 1));
  }

  void operator delete(void *p) { ::operator delete(p); }

  static std::unique_ptr<DefaultMemoryBuffer>
  createMemoryBuffer(std::size_t Size, const std::string &Identifier);
};

} // namespace vc

#endif