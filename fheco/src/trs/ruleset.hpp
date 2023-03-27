#pragma once

#include "trs.hpp"
#include "trs_util_functions.hpp"
#include <vector>

namespace fheco_trs
{
class Ruleset
{
public:
  static std::vector<RewriteRule> rules;
};
} // namespace fheco_trs
