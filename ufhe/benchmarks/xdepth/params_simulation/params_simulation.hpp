#pragma once

#include "seal/seal.h"
#include <cstddef>
#include <string>
#include <vector>

seal::SEALContext bfv_params_simulation(
  int initial_plain_m_size, int xdepth, int safety_margin, bool use_least_levels = false,
  seal::sec_level_type sec_level = seal::sec_level_type::tc128);

seal::EncryptionParameters bfv_params_heuristic(
  int initial_plain_m_size, int xdepth, seal::sec_level_type sec_level = seal::sec_level_type::tc128,
  bool use_least_levels = false, std::size_t initial_poly_md = 1024, std::size_t max_poly_md = 32768,
  const std::string &err_msg = "");

std::vector<int> test_params(const seal::SEALContext &context, int xdepth);

void increase_poly_md(std::size_t &poly_modulus_degree, std::size_t max_poly_md, const std::string &err_msg);

seal::Modulus create_plain_modulus(
  std::size_t poly_modulus_degree, int initial_size, int max_size, const std::string &err_msg);

void reduce_coeff_modulus_bit_count(std::vector<int> &bit_sizes, int &bit_count, int amount);

int first_biggest_prime_index(const std::vector<int> &bit_sizes);

int last_smallest_prime_index(const std::vector<int> &bit_sizes);

void print_noise_budget_progress(const std::vector<int> &noise_budgets);

void print_parameters(const seal::SEALContext &context);

int ceil_float_div(int a, int b);
