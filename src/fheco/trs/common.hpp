#pragma once

#include "fheco/ir/common.hpp"
#include "fheco/ir/term.hpp"
#include "fheco/trs/term_op_code.hpp"
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <unordered_map>
#include <vector>

namespace fheco::trs
{
// order of definition is important for type deduction (OpCode::deduce_result_type)
enum class TermMatcherType
{
  cipher,
  // cipher or plain
  term,
  plain,
  const_
};

enum class RewriteHeuristic
{
  bottom_up,
  top_down
};

bool operator==(TermMatcherType term_matcher_type, ir::Term::Type term_type);

inline bool operator!=(TermMatcherType term_matcher_type, ir::Term::Type term_type)
{
  return !(term_matcher_type == term_type);
}

ir::OpCode convert_op_code(const TermOpCode &op_code, std::vector<int> generators_vals);

using TermsMetric = std::unordered_map<std::size_t, std::int64_t>;

void count_ctxt_leaves(ir::Term *term, TermsMetric &cache);

std::ostream &operator<<(std::ostream &os, TermMatcherType term_matcher_type);

std::ostream &operator<<(std::ostream &os, RewriteHeuristic rewrite_heuristic);
} // namespace fheco::trs
