#pragma once
#include "program.hpp"
#include <variant>

namespace fheco_passes
{

class ArithmeticExpressionSimplfier
{

private:
  ir::Program *program;

  bool convert_difference_to_sum(const ir::Program::Ptr &node);
  bool compact_sum(const ir::Program::Ptr &node);
  bool compact_product(const ir::Program::Ptr &node);
  bool consolidate_product(const ir::Program::Ptr &node);
  bool simplify_product(const ir::Program::Ptr &node);
  bool simplify_sum(const ir::Program::Ptr &node);
  bool simplify_constants(const ir::Program::Ptr &node);
  ir::Program::Ptr make_scalar_term(int64_t value);
  ir::Program::Ptr make_scalar_term(double value);

public:
  /*
    convert subtraction to addition with multiplication by -1
    compact sum .. flatten
    compact product .. flatten
    consolidate product
    simplify sum ( a + a = 2*a )
    simplify constants
  */

  void apply_rules();
};

} // namespace fheco_passes
