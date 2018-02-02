#ifndef VC_DIAG_DIAGNOSTIC_H
#define VC_DIAG_DIAGNOSTIC_H

#include "Common/SourceLocation.h"
#include <stdint.h>

namespace vc {
enum class DiagID : uint32_t;

class DiagnosticBuilder;

class Diagnostic {
  friend class DiagnosticBuilder;
  DiagID ID;
  SourceLocation Location;

public:
  Diagnostic(DiagID ID) : ID(ID) {}

  const char *getString() const;
  const SourceLocation getLocation() const;
};
}; // namespace vc
#endif