#include "Common/OperatorPrecedence.h"

using namespace vc;

prec::Level vc::getBinaryOperatorPrecedence(TokenKind Kind) {
  switch(Kind) {
    case TokenKind::condition_conversion:
      return prec::Condition;
    case TokenKind::equal:
    case TokenKind::inequal:
    case TokenKind::greater:
    case TokenKind::greater_equal:
    case TokenKind::less:
    case TokenKind::less_equal:
    case TokenKind::matching_equal:
    case TokenKind::matching_inequal:
    case TokenKind::matching_greater:
    case TokenKind::matching_greater_equal:
    case TokenKind::matching_less:
    case TokenKind::matching_less_equal:
      return prec::Condition;
    case TokenKind::kw_sll:
    case TokenKind::kw_srl:
    case TokenKind::kw_sla:
    case TokenKind::kw_sra:
    case TokenKind::kw_rol:
    case TokenKind::kw_ror:
      return prec::Shift;
    case TokenKind::plus:
    case TokenKind::minus:
    case TokenKind::ampersand:
      return prec::Adding;
    case TokenKind::asterisk:
    case TokenKind::slash:
    case TokenKind::kw_mod:
    case TokenKind::kw_rem:
      return prec::Multiplying;
    case TokenKind::double_star:
      return prec::Miscellaneous;
    default:
      return prec::Unknown;
  };
}