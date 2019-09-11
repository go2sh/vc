#include "FormatLineFormater.h"
#include "FormatTokenIndenter.h"

using namespace vc;
using namespace format;

namespace detail {
class LineFormater {

public:
  LineFormater(FormatLineFormater &Formater, FormatTokenIndenter &Indenter)
      : Formater(Formater), Indenter(Indenter) {}
  virtual ~LineFormater() = default;

  virtual Penalty formatLine(const FormatLine &Line) = 0;

protected:
  FormatTokenIndenter &Indenter;
  FormatLineFormater &Formater;
};

class NoLineBreakLineFormater : public LineFormater {
public:
  NoLineBreakLineFormater(FormatLineFormater &Formater,
                          FormatTokenIndenter &Indenter)
      : LineFormater(Formater, Indenter) {}

  virtual Penalty formatLine(const FormatLine &Line) {
    LineState State{Line};
    while (State.CurrentToken != nullptr) {
      bool Newline = State.CurrentToken->MustBreakBefore ||
                     (State.CurrentToken->CanBreakBefore &&
                      State.CurrentToken->NewLinesBefore > 0);

      if (State.CurrentToken->Previous) {
        Formater.format(State.CurrentToken->Previous->Children);
      }
      Indenter.addTokenToState(State, Newline);
    }
    return Penalty(0);
  }
};
} // namespace detail

Penalty FormatLineFormater::format(const std::vector<FormatLine *> &Lines) {
  FormatTokenIndenter Indenter{Style, Manager};

  for (auto Line : Lines) {
    detail::NoLineBreakLineFormater Formater{*this, Indenter};
    Formater.formatLine(*Line);
  }
  return Penalty(0);
}