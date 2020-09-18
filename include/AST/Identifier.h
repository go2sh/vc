#ifndef VC_PARSE_IDENTIFIER_H
#define VC_PARSE_IDENTIFIER_H
#include <string>
#include <string_view>
#include <variant>

namespace vc {

struct IdentifierTraits : public std::char_traits<char> {
  static auto to_upper(char ch) -> char { return std::toupper(ch); }
  static auto eq(char c1, char c2) -> bool {
    return to_upper(c1) == to_upper(c2);
  }
  static auto lt(char c1, char c2) -> bool {
    return to_upper(c1) < to_upper(c2);
  }
  static auto compare(const char *s1, const char *s2, std::size_t n) -> int {
    while (n-- != 0) {
      if (to_upper(*s1) < to_upper(*s2))
        return -1;
      if (to_upper(*s1) > to_upper(*s2))
        return 1;
      ++s1;
      ++s2;
    }
    return 0;
  }
  static auto find(const char *s, std::size_t n, char a) -> char const * {
    auto const ua(to_upper(a));
    while (n-- != 0) {
      if (to_upper(*s) == ua)
        return s;
      s++;
    }
    return nullptr;
  }
};

using BasicIdentifier = std::basic_string_view<char, IdentifierTraits>;
using ExtendedIdentifier = std::string_view;
using Identifier = std::variant<BasicIdentifier, ExtendedIdentifier>;

} // namespace vc

#endif // !VC_PARSE_IDENTIFIER_H