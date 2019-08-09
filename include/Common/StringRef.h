#ifndef VC_BASIC_STRINGREF_H
#define VC_BASIC_STRINGREF_H

#include <cassert>
#include <cstddef>
#include <cstring>
#include <functional>
#include <ostream>
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

  typedef const char *const_iterator;

  /*
   * Iterators
   */
  const_iterator begin() { return Data; }
  const_iterator end() { return Data + Length; }
  const_iterator cbegin() { return begin(); }
  const_iterator cend() { return end(); }

  /*
   * std::string like functions
   */
  const char *data() const { return Data; }
  bool empty() const { return Length == 0; }
  size_t size() const { return Length; }
  size_t length() const { return Length; }
  StringRef substr(std::size_t Pos, std::size_t Len = StringRef::npos) const;

  bool equals(StringRef S) const {
    return Length == S.Length &&
           (Length == 0 || std::memcmp(Data, S.Data, Length) == 0);
  }

  /*
   * Search
   */
  std::size_t find(char C) const;
  std::size_t find_if(std::function<bool(const char)> MatchFunc) const;
  std::size_t find_if_not(std::function<bool(const char)> MatchFunc) const;

  /*
   * Trim
   */
  StringRef rtrim(StringRef Chars) const;

  /*
   * String functions
   */
  std::string str() const {
    if (!Data)
      return std::string();
    return std::string(Data, Length);
  }

  /*
   * Operators
   */
  inline const char operator[](size_t Index) const {
    assert(Index < Length && "Index out of range");
    return *(Data + Index);
  }
  template <typename T>
  typename std::enable_if<std::is_same<T, std::string>::value, StringRef>::type
      &
      operator=(T &&Str) = delete;

  /*
   * Type conversion
   */
  operator std::string() const { return str(); }
};

inline bool operator==(StringRef LHS, StringRef RHS) { return LHS.equals(RHS); }
inline void operator<<(std::ostream &LHS, StringRef RHS) {
  LHS.write(RHS.data(), RHS.length());
}
} // namespace vc
#endif