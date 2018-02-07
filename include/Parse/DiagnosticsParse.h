#include "Diag/Diagnostic.h"

namespace vc {
    namespace diag {
        #define DIAG(Type, ID, Text) \
        extern vc::DiagID ID;
        #include <Parse/DiagnosticsParse.def>
    }
}