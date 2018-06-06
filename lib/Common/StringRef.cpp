#include "Common/StringRef.h"

#include <stdexcept>

using namespace vc;

std::size_t StringRef::find(char C) const {
  std::size_t Pos = 0;

  while (Pos < Length) {
    if (*(Data + Pos) == C)
      return Pos;
    Pos++;
  }

  return npos;
}

std::size_t
StringRef::find_if(std::function<bool(const char)> MatchFunc) const {
  std::size_t Pos = 0;

  while (Pos < Length) {
    if (!MatchFunc(*(Data + Pos)))
      return Pos;
    Pos++;
  }

  return npos;
}

std::size_t
StringRef::find_if_not(std::function<bool(const char)> MatchFunc) const {
  std::size_t Pos = 0;

  while (Pos < Length) {
    if (MatchFunc(*(Data + Pos)))
      return Pos;
    Pos++;
  }

  return npos;
}

StringRef StringRef::substr(size_t Pos, size_t Len) const {
  if (Len == StringRef::npos)
    Len = size();
  if (Pos > size())
    throw std::out_of_range("Pos");
  if (Pos == size())
    return StringRef();
  if (Pos + Len > size())
    Len = size() - Pos;

  return StringRef(data() + Pos, Len);
}
