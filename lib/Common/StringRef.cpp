#include "Common/StringRef.h"

#include <algorithm>
#include <cstddef>
#include <stdexcept>

using namespace vc;

auto StringRef::find(char C) const -> std::size_t {
  std::size_t Pos = 0;

  while (Pos < Length) {
    if (*(Data + Pos) == C)
      return Pos;
    Pos++;
  }

  return npos;
}

auto StringRef::find_if(std::function<bool(const char)> MatchFunc) const
    -> std::size_t {
  std::size_t Pos = 0;

  while (Pos < Length) {
    if (!MatchFunc(*(Data + Pos)))
      return Pos;
    Pos++;
  }

  return npos;
}

auto StringRef::find_if_not(std::function<bool(const char)> MatchFunc) const
    -> std::size_t {
  std::size_t Pos = 0;

  const char *C = std::find_if_not(cbegin(), cend(), MatchFunc);

  return npos;
}

auto StringRef::rtrim(StringRef Chars) const -> StringRef {
  const char *C = std::find(cbegin(), cend(), [&Chars](char C) -> bool {
    return Chars.find(C) != StringRef::npos;
  });
  return StringRef(Data, C);
}

auto StringRef::substr(size_t Pos, size_t Len) const -> StringRef {
  if (Len == StringRef::npos) {
    Len = size();
  }
  if (Pos > size()) {
    throw std::out_of_range("Pos");
  }
  if (Pos == size()) {
    return StringRef();
  }
  if (Pos + Len > size()) {
    Len = size() - Pos;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  return StringRef(data() + Pos, Len);
}

inline auto vc::StringRef::operator[](size_t Index) const -> char {
  assert(Index < Length && "Index out of range");

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  return *(Data + Index);
}