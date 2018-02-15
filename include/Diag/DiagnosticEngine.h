#ifndef VC_DIAG_DIAGNOSTICENGINE_H
#define VC_DIAG_DIAGNOSTICENGINE_H

#include "Diag/Diagnostic.h"
#include "Diag/DiagnosticConsumer.h"
#include "Common/StringRef.h"

#include <vector>
#include <ostream>

namespace vc {
class DiagnosticEngine;

class DiagnosticBuilder {
  friend class DiagnosticEngine;
  DiagnosticEngine *Engine;
  Diagnostic Diag;

  DiagnosticBuilder(DiagnosticEngine &Engine, DiagID ID)
      : Engine(&Engine), Diag(ID) {}

  // DiagnosticBuilder(const DiagnosticBuilder &) = delete;
  // DiagnosticBuilder &operator=(const DiagnosticBuilder &) = delete;
  // DiagnosticBuilder &operator=(DiagnosticBuilder &&) = delete;

public:
  DiagnosticBuilder(DiagID ID) : Engine(0), Diag(ID) {}
  ~DiagnosticBuilder() { flush(); }

  void setLocation(SourceLocation Loc) { Diag.Location = Loc; }

private:
  void flush();
};

class DiagnosticEngine {
  friend class DiagnosticBuilder;
  std::vector<DiagnosticConsumer *> Consumers;

public:
  void addConsumer(DiagnosticConsumer *Consumer) {
    Consumers.push_back(Consumer);
  }

  DiagnosticBuilder diagnose(DiagID ID) { return DiagnosticBuilder(*this, ID); }

  static void formatDiagnosticText(std::ostream &Out, StringRef Text, const std::vector<DiagnosticArgument> &Args);

private:
  void emitDiagnostic(const Diagnostic &D);
  void processDiagnostic(const Diagnostic &D);
};

}; // namespace vc

#endif