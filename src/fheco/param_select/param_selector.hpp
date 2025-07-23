#pragma once

#include "fheco/param_select/enc_params.hpp"
#include "fheco/ir/func.hpp"
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory> 
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fheco::param_select
{

class ParameterSelector
{
public:
  ParameterSelector(const std::shared_ptr<ir::Func> &program, EncParams::SecurityLevel sec_level)
    : program_(program), sec_level_(sec_level)
  {}

   EncParams select_params(bool &use_mod_switch);

  struct NoiseEstimatesValue
  {
    int fresh_noise = 0;
    int mul_noise_growth = 0;
    int mul_plain_noise_growth = 0;
  };

private:
 
  EncParams select_params_bfv(bool &use_mod_switch);

  int simulate_noise_bfv(
    NoiseEstimatesValue noise_estimates_value, std::unordered_map<std::size_t, int> &nodes_noise) const;

  bool insert_mod_switch_bfv(
    const std::vector<int> &data_level_primes_sizes, std::unordered_map<std::string, int> &nodes_noise,
    int safety_margin);

  std::unordered_map<std::string, std::unordered_set<std::string>> get_outputs_composing_nodes() const;

  std::shared_ptr<ir::Func> program_;

  EncParams::SecurityLevel sec_level_;

  static const std::unordered_map<EncParams::SecurityLevel, std::unordered_map<std::size_t, int>> security_standard;

  static const std::map<int, std::map<std::size_t, NoiseEstimatesValue>> bfv_noise_estimates_seal;
};
} // namespace param_selector
