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

  explicit StringRef(const char *Str)
      : Data(Str), Length(Str != nullptr ? std::strlen(Str) : 0) {}
  StringRef(const char *Start, const char *End)
      : Data(Start), Length(End - Start) {}
  StringRef(const char *Data, size_t Length) : Data(Data), Length(Length) {}
  explicit StringRef(const std::string &Str)
      : Data(Str.data()), Length(Str.length()) {}

  static const std::size_t npos = -1;

  using const_iterator = const char *;

  /*
   * Iterators
   */
  auto begin() -> const_iterator { return Data; }
  auto end() -> const_iterator {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return Data + Length;
  }
  [[nodiscard]] auto cbegin() const -> const_iterator { return Data; }
  [[nodiscard]] auto cend() const -> const_iterator {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return Data + Length;
  }

  /*
   * std::string like functions
   */
  [[nodiscard]] auto data() const -> const char * { return Data; }
  [[nodiscard]] auto empty() const -> bool { return Length == 0; }
  [[nodiscard]] auto size() const -> size_t { return Length; }
  [[nodiscard]] auto length() const -> size_t { return Length; }
  [[nodiscard]] auto substr(std::size_t Pos,
                            std::size_t Len = StringRef::npos) const
      -> StringRef;

  [[nodiscard]] auto equals(StringRef S) const -> bool {
    return Length == S.Length &&
           (Length == 0 || std::memcmp(Data, S.Data, Length) == 0);
  }

  /*
   * Search
   */
  [[nodiscard]] auto find(char C) const -> std::size_t;
  auto find_if(std::function<bool(const char)> MatchFunc) const -> std::size_t;
  auto find_if_not(std::function<bool(const char)> MatchFunc) const
      -> std::size_t;

  /*
   * Trim
   */
  [[nodiscard]] auto rtrim(StringRef Chars) const -> StringRef;

  /*
   * String functions
   */
  [[nodiscard]] auto str() const -> std::string {
    if (Data == nullptr) {
      return std::string();
    }
    return std::string(Data, Length);
  }

  /*
   * Operators
   */
  inline auto operator[](size_t Index) const -> char;
  template <typename T>
  auto operator=(T &&Str) ->
      typename std::enable_if<std::is_same<T, std::string>::value,
                              StringRef>::type & = delete;

  /*
   * Type conversion
   */
  explicit operator std::string() const { return str(); }
};

inline auto operator==(StringRef LHS, StringRef RHS) -> bool {
  return LHS.equals(RHS);
}
inline void operator<<(std::ostream &LHS, StringRef RHS) {
  LHS.write(RHS.data(), RHS.length());
}
} // namespace vc
#endif