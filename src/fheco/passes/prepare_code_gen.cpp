#include "fheco/dsl/compiler.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/passes/prepare_code_gen.hpp"
#include "stdexcept"

using namespace std;

namespace fheco::passes
{
void prepare_code_gen(const shared_ptr<ir::Func> &func)
{
  Compiler::disable_order_operands();

  for (auto term_id : func->get_top_sorted_terms_ids())
  {
    auto term = func->data_flow().get_term(term_id);
    if (!term)
      continue;

    if (!term->is_operation())
      continue;

    if (term->type() == ir::Term::Type::plain)
      throw logic_error("backend does not support plain plain evaluation");

    if (term->operands().size() == 1)
      continue;

    if (term->operands().size() == 2)
    {
      auto arg1 = term->operands()[0];
      auto arg2 = term->operands()[1];
      if (arg1->type() == ir::Term::Type::plain)
      {
        if (term->op_code().commutativity())
        {
          ir::Term *adjusted_term = func->insert_op_term(term->op_code(), {arg2, arg1});
          func->replace_term_with(term, adjusted_term);
        }
        else
        {
          if (term->op_code().type() == ir::OpCode::Type::sub)
          {
            ir::Term *negate_cipher = func->insert_op_term(ir::OpCode::negate, {arg2});
            ir::Term *adjusted_term = func->insert_op_term(ir::OpCode::add, {negate_cipher, arg1});
            func->replace_term_with(term, adjusted_term);
          }
          else
            throw logic_error("unhandled code generation preparation for non commutative binary operation");
        }
      }
    }
    else
      throw logic_error("unhandled operation with arity > 2");
  }
}
} // namespace fheco::passes
