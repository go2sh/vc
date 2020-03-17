#include <algorithm>

#include "FormatCalculator.h"

using namespace vc;
using namespace vc::format;

void FormatCalculator::calculateFormatInformation(FormatLine &Line) {
  for (auto Token : Line.Tokens) {
    Token->MustBreakBefore =
        Token->MustBreakBefore || mustBreakBefore(Line, *Token);
    Token->CanBreakBefore =
        Token->MustBreakBefore || canBreakBefore(Line, *Token);
    Token->SpacesRequiredBefore = spaceRequiredBefore(Line, *Token);
    Token->NewLinesBefore = newLinesBefore(Line, *Token);
    for (auto* ChildLine : Token->Children) {
      calculateFormatInformation(*ChildLine);
    }
  }
}

bool FormatCalculator::mustBreakBefore(FormatLine &Line, FormatToken &Token) {
  if (Token.Previous && Token.Previous->is(tok::comment)) {
    return true;
  }

  return false;
}

bool FormatCalculator::canBreakBefore(FormatLine &Line, FormatToken &Token) {
  /* Dont break generic/port map/clause and left parenthesis*/
  if (Token.is(tok::left_parenthesis) && Token.Previous &&
      Token.Previous->isAny(tok::kw_generic, tok::kw_port, tok::kw_map)) {
    return false;
  }
  if (Token.is(tok::kw_map)) {
    return false;
  }

  if (Token.is(tok::right_parenthesis) &&
      Style.ClosingBrackets == FormatStyle::ClosingBracketsStyle::NoNewline) {
    return false;
  }
  return true;
}

bool FormatCalculator::spaceRequiredBefore(FormatLine &Line,
                                           FormatToken &Token) {
  if (Token.is(tok::left_parenthesis)) {
    return false;
  }
  if (Token.is(tok::semicolon)) {
    return false;
  }
  if (Token.is(tok::right_parenthesis)) {
    return false;
  }
  if (Token.Previous && Token.Previous->is(tok::left_parenthesis)) {
    return false;
  }
  // No space a start of line
  if (!Token.Previous) {
    return false;
  }
  return true;
}

unsigned FormatCalculator::newLinesBefore(const FormatLine &Line,
                                          FormatToken &Token) {
  unsigned NewLinesBefore = 0;

  if (Token.isFirstOnLine()) {
    // Add at least one newline to every token except on the first line
    if (!Line.isFirst) {
      NewLinesBefore = Token.NewLinesBefore == 0 ? 1 : Token.NewLinesBefore;
    } else {
      NewLinesBefore = 0;
    }
  } else if (Token.MustBreakBefore) {
    NewLinesBefore = Token.NewLinesBefore;
  }

  // Limit the number of newlines
  NewLinesBefore = std::min(NewLinesBefore, Style.MaxEmptyLines + 1);
  return NewLinesBefore;
}