#include "FormatLineFormater.h"
#include "Format/FormatTypes.h"
#include "FormatTokenIndenter.h"

using namespace vc;
using namespace format;

namespace detail {
class LineFormater {

public:
  LineFormater(FormatLineFormater &Formater, FormatTokenIndenter &Indenter)
      : Formater(Formater), Indenter(Indenter) {}
  LineFormater(const LineFormater &) = delete;
  LineFormater(LineFormater &&) = delete;
  auto operator=(const LineFormater &) -> LineFormater & = delete;
  auto operator=(LineFormater &&) -> LineFormater & = delete;
  virtual ~LineFormater() = default;

  virtual auto formatLine(const FormatLine &Line) -> Penalty = 0;

protected:
  inline auto format(const std::vector<FormatLine *> &Lines) -> Penalty {
    return Formater.format(Lines);
  }
  inline auto addTokenToState(LineState &State, bool Newline) -> Penalty {
    return Indenter.addTokenToState(State, Newline);
  }

private:
  FormatTokenIndenter &Indenter;
  FormatLineFormater &Formater;
};

class NoLineBreakLineFormater : public LineFormater {
public:
  NoLineBreakLineFormater(FormatLineFormater &Formater,
                          FormatTokenIndenter &Indenter)
      : LineFormater(Formater, Indenter) {}

  auto formatLine(const FormatLine &Line) -> Penalty final {
    LineState State{Line};
    Penalty Penalty{0};
    while (State.CurrentToken != nullptr) {
      bool Newline = State.CurrentToken->MustBreakBefore ||
                     (State.CurrentToken->CanBreakBefore &&
                      State.CurrentToken->NewLinesBefore > 0);

      if (State.CurrentToken->Previous != nullptr) {
        Penalty += format(State.CurrentToken->Previous->Children);
      }
      Penalty += addTokenToState(State, Newline);
    }
    return Penalty;
  }
};
} // namespace detail

auto FormatLineFormater::format(const std::vector<FormatLine *> &Lines)
    -> Penalty {
  FormatTokenIndenter Indenter{Style, Manager};
  Penalty Score{0};

  for (auto *Line : Lines) {
    detail::NoLineBreakLineFormater Formater{*this, Indenter};
    Score += Formater.formatLine(*Line);
  }

  return Score;
}