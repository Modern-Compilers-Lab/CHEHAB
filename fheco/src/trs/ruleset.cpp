#include "ruleset.hpp"
#include <stdexcept>

using namespace fheco_trs;
using namespace std;

MatchingTerm Ruleset::x = MatchingTerm("x", TermType::ciphertext);
MatchingTerm Ruleset::y = MatchingTerm("y", TermType::ciphertext);
MatchingTerm Ruleset::z = MatchingTerm("z", TermType::ciphertext);
MatchingTerm Ruleset::u = MatchingTerm("u", TermType::ciphertext);

MatchingTerm Ruleset::a = MatchingTerm("a", TermType::scalar);

MatchingTerm Ruleset::n = MatchingTerm("n", fheco_trs::TermType::rawData);
MatchingTerm Ruleset::m = MatchingTerm("m", fheco_trs::TermType::rawData);

function<MatchingTerm(const ir::Program *prgm, const RewriteRule::matching_term_ir_node_map &)> Ruleset::
  gen_rhs_scalar_mul_to_sum(const MatchingTerm &a, const MatchingTerm &x)
{
  return
    [&a, &x](const ir::Program *program, const RewriteRule::matching_term_ir_node_map &matching_map) -> MatchingTerm {
      auto it = matching_map.find(a.get_term_id());
      auto table_entry = program->get_const_entry_form_constants_table(it->second->get_label());
      int64_t a_value = get<int64_t>(get<ir::ScalarValue>(*(*table_entry).get().get_const_entry_value().value));
      if (a_value > 0)
      {
        MatchingTerm rhs = x;
        for (int64_t i = 0; i < a_value - 1; ++i)
          rhs = rhs + x;
        return rhs;
      }
      if (a_value < 0)
      {
        MatchingTerm rhs = -x;
        for (int64_t i = 0; i < -a_value + 1; ++i)
          rhs = rhs - x;
        return rhs;
      }
      throw logic_error("scalar mul by 0 to sum");
    };
}

std::vector<RewriteRule> Ruleset::rules = {
  // {a * x, gen_rhs_scalar_mul_to_sum(a, x)},
  // {x * a, gen_rhs_scalar_mul_to_sum(a, x)},
  {(x << n), x, n == 0},
  {x << n << m, x << MatchingTerm::fold((n + m))},
  {(x << n) + (y << n), (x + y) << n},
  {(x << n) + (y << m), (x + (y << MatchingTerm::fold(m - n))) << n, n < m},
  {(x << n) + (y << m), ((x << MatchingTerm::fold(n - m)) + y) << m, n > m}};
