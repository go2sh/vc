#include "Common/CharInfo.h"
#include "Diag/DiagnosticEngine.h"

using namespace vc;

void DiagnosticBuilder::flush() { Engine->processDiagnostic(Diag); }

void DiagnosticEngine::processDiagnostic(const Diagnostic &Diag) {
  emitDiagnostic(Diag);
}

void DiagnosticEngine::emitDiagnostic(const Diagnostic &Diag) {
  for (auto Consumer : Consumers) {
    Consumer->handleDiagnostic(Diag);
  }
}
/* DiagnosticBuilder DiagnosticEngine::diagnose(DiagID ID) {
  return DiagnosticBuilder(*this, ID);
} */

static void formatDiagnosticArgument(std::ostream &Out, const DiagnosticArgument &Arg) {
  switch(Arg.getKind()) {
    case DiagnosticArgumentKind::String:
      Out << Arg.getAsString();
  }
}

void DiagnosticEngine::formatDiagnosticText(std::ostream &Out, StringRef Text, const std::vector<DiagnosticArgument> &Args) {
  while (!Text.empty()) {
    // Find the argument start point
    std::size_t PercentPos = Text.find('%');
    if (PercentPos == StringRef::npos) {
      // Not argument found in string, write it out
      Out.write(Text.data(), Text.size());
      break;
    }

    // Write out the text before the argument
    Out.write(Text.data(),PercentPos);
    Text = Text.substr(PercentPos + 1);

    // Double '%' are treated as single
    if (Text[0] == '%') {
      Out << '%';
      Text = Text.substr(1);
      continue;
    }

    // Parse the argument index
    std::size_t ArgNumPos = Text.find_if(IsNumeric);
    unsigned ArgNum;
    try {
      ArgNum = std::stoul(Text);
    } catch (std::exception e) {
      assert(false && "Failed to convert dignostic argument index");
    }
    assert (ArgNum < Args.size() && "Argument index out of range");
    Text = Text.substr(ArgNumPos);
    formatDiagnosticArgument(Out, Args[ArgNum]);
  }
}