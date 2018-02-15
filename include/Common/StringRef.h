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

  StringRef(const char *Str) : Data(Str), Length(Str ? std::strlen(Str) : 0) {}
  StringRef(const char *Data, size_t Length) : Data(Data), Length(Length) {}
  StringRef(const std::string &Str) : Data(Str.data()), Length(Str.length()) {}

  static const std::size_t npos = -1;

  const char *data() const { return Data; }
  bool empty() const { return Length == 0; }
  size_t size() const { return Length; }

  bool equals(StringRef S) const {
    return Length == S.Length &&
           (Length == 0 || std::memcmp(Data, S.Data, Length) == 0);
  }

  std::size_t find(char C);
};

inline bool operator==(StringRef LHS, StringRef RHS) { return LHS.equals(RHS); }
} // namespace vc
#endif