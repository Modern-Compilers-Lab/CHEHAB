#pragma once

#include "program.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#define MOD_BIT_COUNT_MAX 60

namespace param_selector
{

class ParameterSelector
{
private:
  ir::Program *program;

  static std::unordered_map<fhecompiler::SecurityLevel, std::unordered_map<std::size_t, int>> security_standard;

  static std::vector<std::tuple<int, std::vector<std::tuple<std::size_t, std::tuple<int, int, int>>>>>
    bfv_noise_estimates_seal;

  EncryptionParameters select_params_bfv(bool use_mod_switch = true);

  int simulate_noise_bfv(
    int fresh_noise, int mul_noise_growth, int mul_plain_noise_growth,
    std::unordered_map<std::string, int> &nodes_noise) const;

  void insert_mod_switch_bfv(
    std::vector<int> data_level_primes_sizes, std::unordered_map<std::string, int> &nodes_noise);

public:
  ParameterSelector(ir::Program *program) : program(program) {}

  void select_params();
};
} // namespace param_selector
