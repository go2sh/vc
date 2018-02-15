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

void DiagnosticEngine::formatDiagnosticText(std::ostream &Out, StringRef Text, const std::vector<DiagnosticArgument> &Args) {
  while (!Text.empty()) {
    std::size_t PercentPos = Text.find('%');

    if (PercentPos == StringRef::npos) {
      Out.write(Text.data(), Text.size());
      return;
    }
  }
}