#pragma once

#include "fheco/ir/common.hpp"
#include "fheco/ir/op_code.hpp"
#include "fheco/ir/term.hpp"
#include "fheco/trs/term_op_code.hpp"
#include <cstdint>
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

bool operator==(TermMatcherType term_matcher_type, ir::TermType term_type);

inline bool operator!=(TermMatcherType term_matcher_type, ir::TermType term_type)
{
  return !(term_matcher_type == term_type);
}

ir::OpCode convert_op_code(const TermOpCode &op_code, std::vector<int> generators_vals);

// for now just evaluate op_type
std::int64_t evaluate_op(const ir::OpCode &op_code, const std::vector<ir::Term *> &operands);

using TermsMetric = std::unordered_map<ir::Term *, std::int64_t, ir::Term::HashPtr, ir::Term::EqualPtr>;

void count_ctxt_leaves(ir::Term *term, TermsMetric &dp);
} // namespace fheco::trs
