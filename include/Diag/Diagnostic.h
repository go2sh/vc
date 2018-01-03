#ifndef VC_DIAG_DIAGNOSTIC_H
#define VC_DIAG_DIAGNOSTIC_H

#include <stdint.h>

namespace vc {
enum class DiagID : uint32_t {
  invalid_diagnostic,
#define DIAG(Type, ID, Text) ID
#include <Diag/DiagnosticsAll.def>
};

class Diagnostic {
  DiagID ID;

public:
  Diagnostic(DiagID ID) : ID(ID) {}

  const char *getString() const;
};
}; // namespace vc
#endif