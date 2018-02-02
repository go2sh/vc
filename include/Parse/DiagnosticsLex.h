#ifndef VC_PARSE_DIAGNOSTIC_LEX_H
#define VC_PARSE_DIAGNOSTIC_LEX_H

#include <Diag/Diagnostic.h>

namespace vc {
    namespace diag {
        #define ERROR(ID, Text) \
        extern vc::DiagID ID;
        #include <Parse/DiagnosticsLex.def>
    }
}
#endif
