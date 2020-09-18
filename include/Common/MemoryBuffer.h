#ifndef VC_BASIC_MEMORY_BUFFER_H
#define VC_BASIC_MEMORY_BUFFER_H

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

namespace vc {

namespace fs = std::filesystem;

class MemoryBuffer {
  const char *BufferStart = nullptr;
  const char *BufferEnd = nullptr;

public:
  virtual ~MemoryBuffer();

  const char *getStart() const { return BufferStart; }
  const char *getEnd() const { return BufferEnd; }
  std::size_t getSize() const { return BufferEnd - BufferStart; }

  virtual std::string_view getIdentifier() { return "<invalid buffer>"; }

  std::string_view getBuffer() const { return std::string_view(BufferStart, getSize()); }

  friend bool operator==(const MemoryBuffer &LHS, const MemoryBuffer &RHS) {
    return std::equal(LHS.getStart(), LHS.getEnd(), RHS.getStart(),
                      RHS.getEnd());
  }

protected:
  void init(const char *BufferStart, const char *BufferEnd);

public:
  static std::unique_ptr<MemoryBuffer>
  getMemoryBuffer(std::string_view Data, std::string_view Filename);
  static std::unique_ptr<MemoryBuffer> getFile(std::string_view Filename);
  static std::unique_ptr<MemoryBuffer> getSTDIN();
};

class DefaultMemoryBuffer : public MemoryBuffer {

public:
  using MemoryBuffer::getEnd;
  using MemoryBuffer::getSize;
  using MemoryBuffer::getStart;

  char *getStart() { return const_cast<char *>(MemoryBuffer::getStart()); }
  char *getEnd() { return const_cast<char *>(MemoryBuffer::getEnd()); }

  virtual std::string_view getIdentifier() {
    return std::string_view(reinterpret_cast<const char *>(this + 1));
  }

  void operator delete(void *p) { ::operator delete(p); }

  static std::unique_ptr<DefaultMemoryBuffer>
  createMemoryBuffer(std::size_t Size, const std::string &Identifier);
};

} // namespace vc

#endif