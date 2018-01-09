#ifndef VC_BASIC_STRINGREF_H
#define VC_BASIC_STRINGREF_H

#include <cstddef>
#include <cstring>
#include <string>

namespace vc {
class StringRef {
private:
  const char *Data = nullptr;
  size_t Length = 0;

public:
  StringRef() = default;
  StringRef(std::nullptr_t) = delete;

  StringRef(const char *Data, size_t Length) : Data(Data), Length(Length) {}
  StringRef(const std::string &Str) : Data(Str.data()), Length(Str.length()) {}

  const char *data() const { return Data; }
  bool empty() const { return Length == 0; }
  size_t size() const { return Length; }
};
} // namespace vc
#endif