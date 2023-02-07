#include "fhecompiler/component_orders.hpp"
#include "fhecompiler/rewrite_rule.hpp"
#include "fhecompiler/utils.hpp"
#include <functional>
#include <iostream>
#include <vector>

using namespace std;
using namespace fheco_trs;

int main()
{
  MatchingTerm x("x", TermType::ciphertextType);
  MatchingTerm y("y", TermType::ciphertextType);
  MatchingTerm z("z", TermType::ciphertextType);
  MatchingTerm u("u", TermType::ciphertextType);
  MatchingTerm n(fheco_trs::TermType::rawDataType);
  MatchingTerm m("m", fheco_trs::TermType::rawDataType);

  MatchingTerm raw1(1);
  MatchingTerm raw0(0);

  std::vector<RewriteRule> ruleset = {
    {x * x, square(x)},
    {x * y * x, square(x) * y},
    {x * y * z * x, square(x) * (y * z)},
    {(x << n) * (y << n), (x * y) << n},
    {x << n << m, x << n + m},
    {(x << n) * (y << m), (x << n - m) * y << m},
    {x * y + z * y, (x + z) * y},
    {x + x * y, x * (y + raw1)},
    {(u + x * y) - z * y, (x - z) * y},
    {x * raw0, raw0},
    {x * raw1, x},
    {x * (y * (z * u)), (x * y) * (z * u)}};

  vector<function<relation_type(const MatchingTerm &, const MatchingTerm &)>> component_orders{
    &xdepth_order,
    [](const MatchingTerm &lhs, const MatchingTerm &rhs) -> relation_type {
      return he_op_class_order(lhs, rhs, &is_he_mul);
    },
    [](const MatchingTerm &lhs, const MatchingTerm &rhs) -> relation_type {
      return he_op_class_order(lhs, rhs, &is_he_square);
    },
    [](const MatchingTerm &lhs, const MatchingTerm &rhs) -> relation_type {
      return he_op_class_order(lhs, rhs, &is_he_rotation);
    },
    &he_rotation_steps_order,
    [](const MatchingTerm &lhs, const MatchingTerm &rhs) -> relation_type {
      return he_op_class_order(lhs, rhs, &is_he_add_sub);
    },
  };

  int rule_id = 0;
  for (const RewriteRule &rule : ruleset)
  {
    cout << "rule" << rule_id << ": ";
    relation_type rel;
    int order_id = 0;
    for (const auto &order : component_orders)
    {
      rel = order(rule.get_lhs(), rule.get_rhs());
      if (rel != relation_type::eq)
        break;
      ++order_id;
    }
    if (rel == relation_type::gt)
      cout << "passed with order" << order_id;
    else if (rel == relation_type::lt)
      cout << "did not pass with order" << order_id;
    else
      cout << "undefined order";
    cout << endl;
    ++rule_id;
  }
}
