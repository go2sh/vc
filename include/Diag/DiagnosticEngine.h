#ifndef VC_DIAG_DIAGNOSTICENGINE_H
#define VC_DIAG_DIAGNOSTICENGINE_H

#include "Common/StringRef.h"
#include "Diag/Diagnostic.h"
#include "Diag/DiagnosticConsumer.h"

#include <ostream>
#include <vector>

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

  void addRange(SourceRange Range) { Diag.Ranges.push_back(Range); }
  void addArgument(const DiagnosticArgument &Arg) { Diag.Arguments.push_back(Arg); }

private:
  void flush();
};

inline DiagnosticBuilder &operator<<(DiagnosticBuilder &D, StringRef S) {
  D.addArgument(DiagnosticArgument(S));
  return D;
}

class DiagnosticEngine {
  friend class DiagnosticBuilder;
  std::vector<DiagnosticConsumer *> Consumers;

public:
  void addConsumer(DiagnosticConsumer *Consumer) {
    Consumers.push_back(Consumer);
  }

  DiagnosticBuilder diagnose(DiagID ID) { return DiagnosticBuilder(*this, ID); }

  static void formatDiagnosticText(std::ostream &Out, StringRef Text,
                                   const std::vector<DiagnosticArgument> &Args);

private:
  void emitDiagnostic(const Diagnostic &D);
  void processDiagnostic(const Diagnostic &D);
};

}; // namespace vc

#endif