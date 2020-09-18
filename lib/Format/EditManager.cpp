#include "EditManager.h"

using namespace vc;
using namespace vc::format;

void EditManager::replaceWhitespace(const FormatToken &FT, unsigned NewLines,
                                    unsigned Spaces) {
  std::string Whitespaces = createWhitespace(NewLines, Spaces);

  if (FT.WhitespacePrefix != Whitespaces) {
    Edits.addEdit(SourceEdit(FT.WhitespaceRange, Whitespaces));
  }
}

std::string EditManager::createWhitespace(unsigned NewLines, unsigned Spaces) {
  std::string Result;
  unsigned ResultTabs = 0;
  unsigned ResultSpaces = 0;

  if (Style.UseTabs == FormatStyle::UseTabsStyle::Never) {
    ResultSpaces = Spaces;
  } else if (Style.UseTabs == FormatStyle::UseTabsStyle::ForIdention) {
    ResultTabs = Spaces / Style.TabSize;
    ResultSpaces = Spaces % Style.TabSize;
  }

  Result.reserve(NewLines + ResultTabs + ResultSpaces);
  Result.append(NewLines, '\n');
  Result.append(ResultTabs, '\t');
  Result.append(ResultSpaces, ' ');

  return Result;
}