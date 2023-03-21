#pragma once

#include "encryption_parameters.hpp"
#include "program.hpp"
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace param_selector
{

class ParameterSelector
{
public:
  ParameterSelector(const std::shared_ptr<ir::Program> &program, fhecompiler::SecurityLevel sec_level)
    : program_(program), sec_level_(sec_level)
  {}

  EncryptionParameters select_params(bool &use_mod_switch);

private:
  struct NoiseEstimatesValue
  {
    int fresh_noise = 0;
    int mul_noise_growth = 0;
    int mul_plain_noise_growth = 0;
  };

  EncryptionParameters select_params_bfv(bool &use_mod_switch);

  int simulate_noise_bfv(
    NoiseEstimatesValue noise_estimates_value, std::unordered_map<std::string, int> &nodes_noise) const;

  bool insert_mod_switch_bfv(
    const std::vector<int> &data_level_primes_sizes, std::unordered_map<std::string, int> &nodes_noise,
    int safety_margin);

  std::unordered_map<std::string, std::unordered_set<std::string>> get_outputs_composing_nodes() const;

  std::shared_ptr<ir::Program> program_;

  fhecompiler::SecurityLevel sec_level_;

  static const std::unordered_map<fhecompiler::SecurityLevel, std::unordered_map<std::size_t, int>> security_standard;

  static const std::map<int, std::map<std::size_t, NoiseEstimatesValue>> bfv_noise_estimates_seal;
};
} // namespace param_selector
