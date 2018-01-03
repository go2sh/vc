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