#pragma once

#include "seal/seal.h"
#include <cstddef>
#include <string>
#include <tuple>
#include <vector>

#define OUT_HE_STD "corresponding parameters not included in HE security standard"

seal::SEALContext bfv_params_simulation(
  int init_plain_mod_size, int xdepth, int safety_margin, seal::sec_level_type sec_level = seal::sec_level_type::tc128);

std::tuple<seal::EncryptionParameters, int> bfv_params_heuristic(
  int init_plain_mod_size, int xdepth, seal::sec_level_type sec_level = seal::sec_level_type::tc128);

std::vector<int> test_params(const seal::SEALContext &context, int xdepth);

seal::Modulus create_plain_mod(std::size_t poly_mod, int plain_mod_init_size);

void reduce_coeff_mod_size(std::vector<int> &primes_sizes, int &total_size, int amount);

int last_small_prime_index(const std::vector<int> &primes_sizes);

void print_noise_budget_progress(const std::vector<int> &noise_budgets);

void print_parameters(const seal::SEALContext &context);

int ceil_float_div(int a, int b);
