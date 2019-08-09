#include "Common/OperatorPrecedence.h"

using namespace vc;

prec::Level vc::getBinaryOperatorPrecedence(tok::TokenKind Kind) {
  switch(Kind) {
    case tok::condition_conversion:
      return prec::Condition;
    case tok::equal:
    case tok::inequal:
    case tok::greater:
    case tok::greater_equal:
    case tok::less:
    case tok::less_equal:
    case tok::matching_equal:
    case tok::matching_inequal:
    case tok::matching_greater:
    case tok::matching_greater_equal:
    case tok::matching_less:
    case tok::matching_less_equal:
      return prec::Condition;
    case tok::kw_sll:
    case tok::kw_srl:
    case tok::kw_sla:
    case tok::kw_sra:
    case tok::kw_rol:
    case tok::kw_ror:
      return prec::Shift;
    case tok::plus:
    case tok::minus:
    case tok::ampersand:
      return prec::Adding;
    case tok::asterisk:
    case tok::slash:
    case tok::kw_mod:
    case tok::kw_rem:
      return prec::Multiplying;
    case tok::double_star:
      return prec::Miscellaneous;
    default:
      return prec::Unknown;
  };
}