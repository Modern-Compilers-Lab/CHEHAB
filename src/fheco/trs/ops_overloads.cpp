#include "fheco/trs/ops_overloads.hpp"
#include <stdexcept>
#include <utility>

using namespace std;

namespace fheco::trs
{
// addition
TermMatcher operator+(TermMatcher lhs, TermMatcher rhs)
{
  return TermMatcher{TermOpCode::add, vector<TermMatcher>{move(lhs), move(rhs)}};
}

TermMatcher &operator+=(TermMatcher &lhs, TermMatcher rhs)
{
  lhs = lhs + rhs;
  return lhs;
}

OpGenMatcher operator+(OpGenMatcher lhs, OpGenMatcher rhs)
{
  return OpGenMatcher{OpGenOpCode::add, vector<OpGenMatcher>{move(lhs), move(rhs)}};
}

OpGenMatcher &operator+=(OpGenMatcher &lhs, OpGenMatcher rhs)
{
  lhs = lhs + rhs;
  return lhs;
}

// subtraction
TermMatcher operator-(TermMatcher lhs, TermMatcher rhs)
{
  return TermMatcher{TermOpCode::sub, vector<TermMatcher>{move(lhs), move(rhs)}};
}

TermMatcher &operator-=(TermMatcher &lhs, TermMatcher rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

OpGenMatcher operator-(OpGenMatcher lhs, OpGenMatcher rhs)
{
  return OpGenMatcher{OpGenOpCode::sub, vector<OpGenMatcher>{move(lhs), move(rhs)}};
}

OpGenMatcher &operator-=(OpGenMatcher &lhs, OpGenMatcher rhs)
{
  lhs = lhs - rhs;
  return lhs;
}

// multiplication
TermMatcher operator*(TermMatcher lhs, TermMatcher rhs)
{
  return TermMatcher{TermOpCode::mul, vector<TermMatcher>{move(lhs), move(rhs)}};
}

TermMatcher &operator*=(TermMatcher &lhs, TermMatcher rhs)
{
  lhs = lhs * rhs;
  return lhs;
}

// negation
TermMatcher operator-(TermMatcher arg)
{
  return TermMatcher{TermOpCode::negate, vector<TermMatcher>{move(arg)}};
}

OpGenMatcher operator-(OpGenMatcher arg)
{
  return OpGenMatcher{OpGenOpCode::negate, vector<OpGenMatcher>{move(arg)}};
}

// rotation
TermMatcher operator<<(TermMatcher arg, OpGenMatcher steps)
{
  return TermMatcher{TermOpCode::rotate(move(steps)), vector<TermMatcher>{move(arg)}};
}

TermMatcher &operator<<=(TermMatcher &arg, OpGenMatcher steps)
{
  arg = arg << steps;
  return arg;
}

TermMatcher operator>>(TermMatcher arg, OpGenMatcher steps)
{
  return TermMatcher{TermOpCode::rotate(-move(steps)), vector<TermMatcher>{move(arg)}};
}

TermMatcher &operator>>=(TermMatcher &arg, OpGenMatcher steps)
{
  arg = arg >> steps;
  return arg;
}

// square
TermMatcher square(TermMatcher arg)
{
  return TermMatcher{TermOpCode::square, vector<TermMatcher>{move(arg)}};
}

// mod
OpGenMatcher operator%(OpGenMatcher lhs, OpGenMatcher rhs)
{
  return OpGenMatcher{OpGenOpCode::mod, vector<OpGenMatcher>{move(lhs), move(rhs)}};
}

OpGenMatcher &operator%=(OpGenMatcher &lhs, OpGenMatcher rhs)
{
  lhs = lhs % rhs;
  return lhs;
}

// other

TermMatcher balanced_op(const vector<TermMatcher> &args, const TermOpCode &op_code)
{
  if (args.empty())
    throw invalid_argument("empty args vector");

  if (op_code.arity() != 2)
    throw invalid_argument("non-binary operation");

  vector<TermMatcher> balanced_ops;
  for (size_t i = 0; i < args.size() - 1; i += 2)
    balanced_ops.push_back(TermMatcher{op_code, vector<TermMatcher>{args[i], args[i + 1]}});

  if (args.size() & 1)
    balanced_ops.push_back(args.back());

  for (size_t i = 0; i < balanced_ops.size() - 1; i += 2)
    balanced_ops.push_back(TermMatcher{op_code, vector<TermMatcher>{balanced_ops[i], balanced_ops[i + 1]}});

  return balanced_ops.back();
}

TermMatcher relin(TermMatcher arg)
{
  return TermMatcher{TermOpCode::relin, vector<TermMatcher>{move(arg)}};
}
} // namespace fheco::trs
