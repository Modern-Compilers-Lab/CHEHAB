#pragma once

#include <cstddef>
#include <memory>
#include <unordered_set>
#include <vector>

namespace fheco::ir
{
class Func;

class Term;
} // namespace fheco::ir

namespace fheco::passes
{
std::unordered_set<int> reduce_rotation_keys(const std::shared_ptr<ir::Func> &func, std::size_t keys_threshold = 29);

void decomp_rotation_term(const std::shared_ptr<ir::Func> &func, ir::Term *term, const std::vector<int> &steps_seq);

// from SEAL native/src/seal/util/numth.h
std::vector<int> naf(int value);
} // namespace fheco::passes
