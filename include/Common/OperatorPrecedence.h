#ifndef VC_COMMON_OPERATOR_PRECEDENCE_H
#define VC_COMMON_OPERATOR_PRECEDENCE_H

#include "Common/TokenKinds.h"

namespace vc {
namespace prec {
enum Level {
  Unknown = 0,
  Condition = 1,
  Logical = 2,
  Relational = 3,
  Shift = 4,
  Adding = 5,
  Sign = 6,
  Multiplying = 7,
  Miscellaneous = 8
};
}

prec::Level getBinaryOperatorPrecedence(tok::TokenKind Kind);
} // namespace vc

#endif // !VC_COMMON_OPERATOR_PRECEDENCE_H