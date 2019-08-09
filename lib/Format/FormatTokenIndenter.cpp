#include "FormatTokenIndenter.h"

using namespace vc;
using namespace format;

Penalty FormatTokenIndenter::addTokenToState(LineState &State, bool Newline) {

  Penalty TokenPenalty{0};
  if (Newline) {
    TokenPenalty += addTokenOnNewLine(State);
  } else {
    addTokenOnCurrentLine(State);
  }
  return TokenPenalty;
}

Penalty FormatTokenIndenter::addTokenOnNewLine(LineState &State) {
  State.Column = getNewLineColumn(State);
  Manager.replaceWhitespace(*State.CurrentToken,
                            State.CurrentToken->NewLinesBefore, State.Column);

  moveStateToNextToken(State);
  return Penalty{0};
}

void FormatTokenIndenter::addTokenOnCurrentLine(LineState &State) {
  State.Column += State.CurrentToken->SpacesRequiredBefore;

  Manager.replaceWhitespace(*State.CurrentToken, 0,
                            State.CurrentToken->SpacesRequiredBefore);
  moveStateToNextToken(State);
}

unsigned FormatTokenIndenter::getNewLineColumn(LineState &State) {
  if (State.CurrentToken->isFirstOnLine()) {
    return State.Line.Level * Style.IndentionWidth;
  }

  return (State.Line.Level + 1) * Style.IndentionWidth;
}

void FormatTokenIndenter::moveStateToNextToken(LineState &State) {
  State.Column += State.CurrentToken->ColumnWidth;
  State.CurrentToken = State.CurrentToken->Next;
}