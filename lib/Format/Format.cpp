#include <Common/TextBuffer.h>
#include <Format/Format.h>
#include <Support/SourceEdit.h>

#include "EditManager.h"
#include "FormatCalculator.h"
#include "FormatLineFormater.h"
#include "FormatLineParser.h"
#include "FormatTokenLexer.h"

using namespace vc;

void format::reformat(const SourceManager &SrcMgr, SourceFile SF,
                      const FormatStyle &Style) {
  format::FormatTokenLexer FTLexer{SrcMgr, SF, Style};
  format::FormatLineParser FLParser{Style, FTLexer.getTokens()};
  format::FormatCalculator Calc{Style};
  format::EditManager Manager{SF, Style};
  format::FormatLineFormater Formater{Manager, Style};

  FTLexer.lex();
  FLParser.parse();

  std::vector<FormatLine *> &Lines = FLParser.getLines();

  std::for_each(std::begin(Lines), std::end(Lines), [&Calc](FormatLine *Line) {
    Calc.calculateFormatInformation(*Line);
  });

  Penalty FormatPenalty = Formater.format(Lines);

  SourceEdits &Edits = Manager.getSourceEdits();
  const MemoryBuffer *Buf = SrcMgr.getBuffer(SF);
  GapTextBuffer Buffer{Buf->getStart(), Buf->getSize()};
  std::for_each(
      std::rbegin(Edits), std::rend(Edits),
      [&Buffer, &SrcMgr, SF](const SourceEdit &Edit) {
        SourceLocation StartOfFile = SrcMgr.getStartOfFile(SF);
        if (Edit.isInsert()) {
          Buffer.insertText(Edit.getRange().getBegin().getRawEnconding() -
                                StartOfFile.getRawEnconding(),
                            Edit.getText().data(), Edit.getText().length());
        }
        if (Edit.isReplace()) {
          Buffer.replaceText(Edit.getRange().getBegin().getRawEnconding() -
                                 StartOfFile.getRawEnconding(),
                             Edit.getRange().length(), Edit.getText().data(),
                             Edit.getText().length());
        }
      });
  std::cout << Buffer.text();
  return;
}