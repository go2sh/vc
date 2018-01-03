#ifndef VC_DIAG_DIAGNOSTICCONSUMER_H
#define VC_DIAG_DIAGNOSTICCONSUMER_H

#include "Diagnostic.h"
namespace vc {
class DiagnosticConsumer {

public:
  virtual void handleDiagnostic(const Diagnostic &Diag) = 0;
};
}; // namespace vc
#endif