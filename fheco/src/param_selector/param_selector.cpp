#include "param_selector.hpp"
#include "fhecompiler_const.hpp"
#include "term.hpp"
#include <iostream>
#include <stdexcept>

using namespace std;

namespace param_selector
{

const unordered_map<fhecompiler::SecurityLevel, unordered_map<size_t, int>> ParameterSelector::security_standard = {
  {fhecompiler::SecurityLevel::tc128, {{1024, 27}, {2048, 54}, {4096, 109}, {8192, 218}, {16384, 438}, {32768, 881}}},
  {fhecompiler::SecurityLevel::tc192, {{1024, 19}, {2048, 37}, {4096, 75}, {8192, 152}, {16384, 305}, {32768, 612}}},
  {fhecompiler::SecurityLevel::tc256, {{1024, 14}, {2048, 29}, {4096, 58}, {8192, 118}, {16384, 237}, {32768, 476}}}};

const map<int, map<size_t, ParameterSelector::NoiseEstimatesValue>> ParameterSelector::bfv_noise_estimates_seal = {
  {14, {{1024, {6, 25, 18}}, {2048, {7, 26, 18}}}},
  {15, {{1024, {6, 25, 18}}}},
  {16, {{1024, {6, 27, 20}}, {2048, {7, 28, 20}}, {4096, {7, 28, 20}}}},
  {17,
   {{1024, {6, 28, 21}},
    {2048, {7, 29, 21}},
    {4096, {7, 30, 22}},
    {8192, {8, 31, 22}},
    {16384, {8, 31, 22}},
    {32768, {8, 32, 22}}}},
  {18, {{1024, {6, 29, 22}}, {2048, {7, 30, 22}}, {4096, {7, 30, 22}}, {8192, {7, 31, 23}}, {16384, {8, 32, 23}}}},
  {19, {{1024, {6, 30, 23}}, {2048, {7, 31, 23}}, {4096, {7, 31, 24}}}},
  {20,
   {{1024, {6, 31, 24}},
    {2048, {7, 32, 25}},
    {4096, {8, 33, 25}},
    {8192, {8, 34, 25}},
    {16384, {8, 35, 25}},
    {32768, {9, 35, 26}}}},
  {21,
   {{1024, {6, 32, 25}},
    {2048, {7, 34, 25}},
    {4096, {7, 34, 26}},
    {8192, {8, 35, 26}},
    {16384, {8, 36, 27}},
    {32768, {9, 37, 27}}}},
  {22,
   {{1024, {6, 33, 26}},
    {2048, {7, 34, 26}},
    {4096, {7, 35, 27}},
    {8192, {8, 36, 28}},
    {16384, {8, 37, 28}},
    {32768, {9, 38, 29}}}},
  {23,
   {{1024, {6, 34, 27}},
    {2048, {7, 35, 27}},
    {4096, {8, 36, 28}},
    {8192, {8, 38, 28}},
    {16384, {9, 38, 29}},
    {32768, {9, 39, 29}}}},
  {24,
   {{1024, {6, 35, 28}},
    {2048, {7, 36, 28}},
    {4096, {7, 37, 29}},
    {8192, {8, 38, 29}},
    {16384, {9, 39, 30}},
    {32768, {9, 40, 30}}}},
  {25,
   {{1024, {6, 36, 30}},
    {2048, {7, 38, 30}},
    {4096, {8, 38, 30}},
    {8192, {8, 39, 31}},
    {16384, {9, 40, 31}},
    {32768, {9, 41, 31}}}},
  {26,
   {{1024, {6, 37, 30}},
    {2048, {7, 38, 31}},
    {4096, {7, 39, 31}},
    {8192, {8, 40, 31}},
    {16384, {9, 41, 32}},
    {32768, {9, 42, 32}}}},
  {27,
   {{1024, {6, 38, 31}},
    {2048, {7, 40, 31}},
    {4096, {8, 40, 32}},
    {8192, {8, 41, 32}},
    {16384, {9, 42, 33}},
    {32768, {9, 43, 33}}}},
  {28,
   {{1024, {6, 39, 32}},
    {2048, {7, 40, 33}},
    {4096, {7, 41, 33}},
    {8192, {8, 42, 33}},
    {16384, {9, 43, 34}},
    {32768, {9, 44, 34}}}},
  {29,
   {{1024, {6, 40, 33}},
    {2048, {7, 41, 34}},
    {4096, {8, 43, 34}},
    {8192, {8, 43, 34}},
    {16384, {9, 44, 35}},
    {32768, {9, 45, 36}}}},
  {30,
   {{1024, {6, 41, 34}},
    {2048, {7, 42, 35}},
    {4096, {7, 43, 35}},
    {8192, {8, 44, 36}},
    {16384, {9, 45, 36}},
    {32768, {9, 46, 36}}}},
  {31,
   {{1024, {6, 42, 35}},
    {2048, {7, 43, 36}},
    {4096, {8, 44, 36}},
    {8192, {8, 45, 37}},
    {16384, {9, 46, 37}},
    {32768, {9, 47, 37}}}},
  {32,
   {{1024, {7, 43, 36}},
    {2048, {7, 45, 36}},
    {4096, {7, 45, 37}},
    {8192, {8, 46, 37}},
    {16384, {9, 47, 38}},
    {32768, {9, 48, 38}}}},
  {33,
   {{1024, {7, 44, 37}},
    {2048, {7, 45, 37}},
    {4096, {7, 46, 38}},
    {8192, {8, 47, 38}},
    {16384, {9, 48, 39}},
    {32768, {9, 49, 39}}}},
  {34,
   {{1024, {6, 45, 38}},
    {2048, {7, 46, 39}},
    {4096, {7, 47, 39}},
    {8192, {8, 48, 40}},
    {16384, {9, 49, 40}},
    {32768, {9, 50, 41}}}},
  {35,
   {{1024, {6, 46, 39}},
    {2048, {7, 47, 39}},
    {4096, {8, 48, 40}},
    {8192, {8, 49, 40}},
    {16384, {9, 51, 41}},
    {32768, {9, 51, 41}}}},
  {36,
   {{1024, {6, 47, 40}},
    {2048, {7, 48, 41}},
    {4096, {8, 49, 41}},
    {8192, {8, 50, 41}},
    {16384, {9, 51, 42}},
    {32768, {9, 52, 42}}}},
  {37,
   {{1024, {6, 48, 41}},
    {2048, {7, 49, 42}},
    {4096, {7, 50, 42}},
    {8192, {8, 52, 43}},
    {16384, {9, 52, 43}},
    {32768, {9, 53, 43}}}},
  {38,
   {{1024, {6, 50, 42}},
    {2048, {7, 50, 43}},
    {4096, {8, 51, 43}},
    {8192, {8, 52, 43}},
    {16384, {9, 53, 44}},
    {32768, {9, 54, 44}}}},
  {39,
   {{1024, {6, 50, 43}},
    {2048, {7, 51, 44}},
    {4096, {8, 52, 44}},
    {8192, {8, 53, 44}},
    {16384, {9, 54, 45}},
    {32768, {9, 55, 45}}}},
  {40,
   {{1024, {7, 51, 44}},
    {2048, {7, 52, 44}},
    {4096, {7, 53, 45}},
    {8192, {8, 54, 45}},
    {16384, {9, 55, 46}},
    {32768, {9, 56, 46}}}},
  {41,
   {{1024, {6, 53, 45}},
    {2048, {7, 53, 46}},
    {4096, {8, 54, 46}},
    {8192, {8, 55, 47}},
    {16384, {9, 56, 47}},
    {32768, {9, 58, 47}}}},
  {42,
   {{1024, {7, 53, 46}},
    {2048, {7, 54, 47}},
    {4096, {7, 55, 47}},
    {8192, {8, 56, 48}},
    {16384, {8, 57, 48}},
    {32768, {9, 58, 48}}}},
  {43,
   {{1024, {6, 54, 47}},
    {2048, {7, 55, 47}},
    {4096, {7, 56, 48}},
    {8192, {8, 57, 48}},
    {16384, {9, 58, 49}},
    {32768, {9, 59, 49}}}},
  {44,
   {{1024, {6, 55, 48}},
    {2048, {7, 56, 49}},
    {4096, {8, 57, 49}},
    {8192, {8, 58, 49}},
    {16384, {9, 59, 50}},
    {32768, {9, 60, 50}}}},
  {45,
   {{1024, {6, 56, 49}},
    {2048, {7, 57, 49}},
    {4096, {8, 58, 50}},
    {8192, {8, 59, 51}},
    {16384, {9, 60, 51}},
    {32768, {9, 61, 52}}}},
  {46,
   {{1024, {6, 57, 50}},
    {2048, {7, 58, 51}},
    {4096, {7, 60, 51}},
    {8192, {8, 60, 51}},
    {16384, {8, 61, 52}},
    {32768, {9, 63, 53}}}},
  {47,
   {{1024, {7, 58, 51}},
    {2048, {7, 59, 51}},
    {4096, {7, 60, 52}},
    {8192, {8, 61, 52}},
    {16384, {9, 62, 53}},
    {32768, {9, 63, 53}}}},
  {48,
   {{1024, {6, 59, 52}},
    {2048, {7, 60, 52}},
    {4096, {8, 61, 53}},
    {8192, {8, 62, 53}},
    {16384, {9, 63, 54}},
    {32768, {9, 64, 54}}}},
  {49,
   {{1024, {6, 60, 53}},
    {2048, {7, 61, 54}},
    {4096, {8, 62, 54}},
    {8192, {8, 63, 54}},
    {16384, {9, 64, 55}},
    {32768, {9, 65, 55}}}},
  {50,
   {{1024, {6, 61, 54}},
    {2048, {7, 62, 54}},
    {4096, {7, 63, 55}},
    {8192, {8, 64, 55}},
    {16384, {9, 65, 56}},
    {32768, {9, 67, 56}}}},
  {51,
   {{1024, {6, 62, 56}},
    {2048, {7, 63, 55}},
    {4096, {8, 64, 56}},
    {8192, {8, 65, 56}},
    {16384, {9, 66, 57}},
    {32768, {9, 67, 58}}}},
  {52,
   {{1024, {6, 63, 56}},
    {2048, {7, 65, 56}},
    {4096, {8, 65, 57}},
    {8192, {8, 66, 58}},
    {16384, {9, 67, 58}},
    {32768, {9, 68, 58}}}},
  {53,
   {{1024, {6, 64, 57}},
    {2048, {7, 65, 58}},
    {4096, {8, 66, 58}},
    {8192, {8, 67, 59}},
    {16384, {9, 68, 59}},
    {32768, {9, 69, 59}}}},
  {54,
   {{1024, {7, 65, 58}},
    {2048, {7, 66, 59}},
    {4096, {7, 67, 59}},
    {8192, {8, 68, 59}},
    {16384, {9, 69, 60}},
    {32768, {9, 70, 60}}}},
  {55,
   {{1024, {7, 66, 59}},
    {2048, {7, 67, 59}},
    {4096, {7, 68, 60}},
    {8192, {8, 69, 61}},
    {16384, {9, 70, 61}},
    {32768, {9, 71, 61}}}},
  {56,
   {{1024, {7, 67, 60}},
    {2048, {7, 69, 61}},
    {4096, {8, 69, 61}},
    {8192, {8, 70, 61}},
    {16384, {9, 71, 62}},
    {32768, {9, 72, 62}}}},
  {57,
   {{1024, {6, 68, 61}},
    {2048, {7, 69, 61}},
    {4096, {8, 70, 62}},
    {8192, {8, 71, 63}},
    {16384, {9, 72, 63}},
    {32768, {9, 74, 63}}}},
  {58,
   {{1024, {6, 69, 62}},
    {2048, {7, 70, 62}},
    {4096, {7, 71, 63}},
    {8192, {8, 72, 63}},
    {16384, {9, 73, 64}},
    {32768, {9, 74, 64}}}},
  {59,
   {{1024, {6, 70, 63}},
    {2048, {7, 71, 64}},
    {4096, {7, 72, 64}},
    {8192, {8, 73, 64}},
    {16384, {9, 74, 65}},
    {32768, {9, 75, 65}}}},
  {60,
   {{1024, {6, 71, 64}},
    {2048, {7, 72, 65}},
    {4096, {7, 73, 66}},
    {8192, {8, 74, 65}},
    {16384, {9, 75, 66}},
    {32768, {9, 76, 66}}}}};

EncryptionParameters ParameterSelector::select_params(bool &use_mod_switch)
{
  program_->get_dataflow_sorted_nodes(true);

  switch (program_->get_encryption_scheme())
  {
  case fhecompiler::Scheme::bfv:
    return select_params_bfv(use_mod_switch);
    break;

  case fhecompiler::Scheme::none:
    throw logic_error("no shceme was specified");

  default:
    throw logic_error("parameter selection unsupported for the scheme");
    break;
  }
}

EncryptionParameters ParameterSelector::select_params_bfv(bool &use_mod_switch)
{
  int plain_mod_size = program_->get_bit_width() + 1;
  if (program_->get_signedness())
    ++plain_mod_size;

  auto plain_mod_noise_estimates_it = bfv_noise_estimates_seal.find(plain_mod_size);
  while (plain_mod_noise_estimates_it == bfv_noise_estimates_seal.end() &&
         plain_mod_size < bfv_noise_estimates_seal.rbegin()->first && plain_mod_size < MOD_BIT_COUNT_MAX)
  {
    ++plain_mod_size;
    plain_mod_noise_estimates_it = bfv_noise_estimates_seal.find(plain_mod_size);
  }
  if (plain_mod_noise_estimates_it == bfv_noise_estimates_seal.end())
    throw logic_error("noise estimates maximum plaintext modulus size smaller than bit_width+1+signedness");

  if (plain_mod_noise_estimates_it->second.empty())
    throw logic_error("empty per polynomial modulus degree estimates map for the given plaintext modulus size");

  size_t poly_modulus_degree = program_->get_vector_size() << 1;
  auto poly_modulus_degree_noise_estimates_it = plain_mod_noise_estimates_it->second.find(poly_modulus_degree);

  while (poly_modulus_degree_noise_estimates_it == plain_mod_noise_estimates_it->second.end() &&
         poly_modulus_degree < plain_mod_noise_estimates_it->second.rbegin()->first)
  {
    poly_modulus_degree = poly_modulus_degree << 1;
    poly_modulus_degree_noise_estimates_it = plain_mod_noise_estimates_it->second.find(poly_modulus_degree);
  }
  if (poly_modulus_degree_noise_estimates_it == plain_mod_noise_estimates_it->second.end())
    throw logic_error("the maximum polynomial modulus degree of the noise estimates for the given plaintext modulus "
                      "size is smaller than vector_size");
  int circuit_noise = 0;
  EncryptionParameters params;
  unordered_map<string, int> nodes_noise;
  while (poly_modulus_degree_noise_estimates_it != plain_mod_noise_estimates_it->second.end())
  {
    poly_modulus_degree = poly_modulus_degree_noise_estimates_it->first;
    const NoiseEstimatesValue &noise_estimates_value = poly_modulus_degree_noise_estimates_it->second;
    nodes_noise.clear();
    circuit_noise = simulate_noise_bfv(noise_estimates_value, nodes_noise);

    int coeff_mod_data_level_size = plain_mod_size + circuit_noise;
    params = EncryptionParameters(poly_modulus_degree, plain_mod_size, coeff_mod_data_level_size);

    if (sec_level_ == fhecompiler::SecurityLevel::none)
      break;

    auto sec_level_he_standard_it = security_standard.find(sec_level_);
    if (sec_level_he_standard_it == security_standard.end())
      throw logic_error("program security level is not included in the HE security standard");

    auto poly_modulus_he_standard_it = sec_level_he_standard_it->second.find(poly_modulus_degree);
    if (poly_modulus_he_standard_it == sec_level_he_standard_it->second.end())
      throw logic_error("the reached polynomial modulus degree is not included in the HE security standard");

    if (params.coeff_mod_bit_count() <= poly_modulus_he_standard_it->second)
      break;

    ++poly_modulus_degree_noise_estimates_it;
  }
  if (poly_modulus_degree_noise_estimates_it == plain_mod_noise_estimates_it->second.end())
    throw logic_error("could not find suitable parameters");

  cout << "estimated circuit_noise: " << circuit_noise << '\n';
  cout << "-> q: "
       << "nb_primes=" << params.coeff_mod_bit_sizes().size() << ", "
       << "min_total_bit_count=" << params.coeff_mod_bit_count() << '\n';

  if (sec_level_ != fhecompiler::SecurityLevel::none)
  {
    auto security_standard_sec_level_it = security_standard.find(sec_level_);
    if (security_standard_sec_level_it == security_standard.end())
      throw logic_error("sec_level_ not included in security_standard");

    auto security_standard_poly_modulus_degree_it = security_standard_sec_level_it->second.find(poly_modulus_degree);
    if (security_standard_poly_modulus_degree_it == security_standard_sec_level_it->second.end())
      throw logic_error("poly_modulus_degree not included in security_standard");

    int max_coeff_mod_bit_count = security_standard_poly_modulus_degree_it->second;
    params.increase_coeff_mod_bit_sizes(max_coeff_mod_bit_count - params.coeff_mod_bit_count());
  }
  else
    params.increase_coeff_mod_bit_sizes(MOD_BIT_COUNT_MAX);

  if (use_mod_switch)
    use_mod_switch = insert_mod_switch_bfv(
      params.coeff_mod_data_level_bit_sizes(), nodes_noise, poly_modulus_degree_noise_estimates_it->second.fresh_noise);

  return params;
}

int ParameterSelector::simulate_noise_bfv(
  NoiseEstimatesValue noise_estimates, unordered_map<string, int> &nodes_noise) const
{
  int fresh_noise = noise_estimates.fresh_noise;
  int mul_noise_growth = noise_estimates.mul_noise_growth;
  int mul_plain_noise_growth = noise_estimates.mul_plain_noise_growth;

  map<tuple<ir::OpCode, ir::TermType, ir::TermType>, int> operations_noise_growth = {
    {{ir::OpCode::mul, ir::TermType::ciphertext, ir::TermType::ciphertext}, mul_noise_growth},
    {{ir::OpCode::square, ir::TermType::ciphertext, ir::TermType::undefined}, mul_noise_growth},
    {{ir::OpCode::mul, ir::TermType::ciphertext, ir::TermType::plaintext}, mul_plain_noise_growth},
    {{ir::OpCode::mul, ir::TermType::plaintext, ir::TermType::ciphertext}, mul_plain_noise_growth},
    {{ir::OpCode::mul, ir::TermType::ciphertext, ir::TermType::scalar}, mul_plain_noise_growth},
    {{ir::OpCode::mul, ir::TermType::scalar, ir::TermType::ciphertext}, mul_plain_noise_growth},
    {{ir::OpCode::add, ir::TermType::ciphertext, ir::TermType::ciphertext}, 1},
    {{ir::OpCode::add, ir::TermType::ciphertext, ir::TermType::plaintext}, 0},
    {{ir::OpCode::add, ir::TermType::plaintext, ir::TermType::ciphertext}, 0},
    {{ir::OpCode::add, ir::TermType::ciphertext, ir::TermType::scalar}, 0},
    {{ir::OpCode::add, ir::TermType::scalar, ir::TermType::ciphertext}, 0},
    {{ir::OpCode::negate, ir::TermType::ciphertext, ir::TermType::undefined}, 1},
    {{ir::OpCode::sub, ir::TermType::ciphertext, ir::TermType::ciphertext}, 1},
    {{ir::OpCode::sub, ir::TermType::ciphertext, ir::TermType::plaintext}, 0},
    {{ir::OpCode::sub, ir::TermType::plaintext, ir::TermType::ciphertext}, 0},
    {{ir::OpCode::sub, ir::TermType::ciphertext, ir::TermType::scalar}, 0},
    {{ir::OpCode::sub, ir::TermType::scalar, ir::TermType::ciphertext}, 0},
    {{ir::OpCode::rotate, ir::TermType::ciphertext, ir::TermType::rawData}, 5},
    {{ir::OpCode::rotate, ir::TermType::rawData, ir::TermType::ciphertext}, 5},
    {{ir::OpCode::assign, ir::TermType::ciphertext, ir::TermType::undefined}, 0},
    {{ir::OpCode::encrypt, ir::TermType::plaintext, ir::TermType::undefined}, fresh_noise},
    {{ir::OpCode::encrypt, ir::TermType::scalar, ir::TermType::undefined}, fresh_noise},
    {{ir::OpCode::relinearize, ir::TermType::ciphertext, ir::TermType::undefined}, 0}};

  int circuit_noise = fresh_noise;

  const auto &nodes = program_->get_dataflow_sorted_nodes(false);
  for (const auto &node : nodes)
  {
    if (node->get_term_type() != ir::TermType::ciphertext)
      continue;

    if (nodes_noise.find(node->get_label()) != nodes_noise.end())
      throw logic_error("repeated node in dataflow_sorted_nodes");

    if (!node->is_operation_node())
      nodes_noise.insert({node->get_label(), fresh_noise});
    else
    {
      int result_noise = 0;
      if (node->get_operands().size() == 2)
      {
        const auto &arg1 = node->get_operands()[0];
        const auto &arg2 = node->get_operands()[1];

        bool is_cipher = false;
        int arg1_noise = 0;
        auto arg1_it = nodes_noise.find(arg1->get_label());
        if (arg1->get_term_type() == ir::TermType::ciphertext)
        {
          if (arg1_it == nodes_noise.end())
            throw logic_error("parent handled before child");

          is_cipher = true;
          arg1_noise = arg1_it->second;
        }
        int arg2_noise = 0;
        auto arg2_it = nodes_noise.find(arg2->get_label());
        if (arg2->get_term_type() == ir::TermType::ciphertext)
        {
          if (arg2_it == nodes_noise.end())
            throw logic_error("parent handled before child");

          is_cipher = true;
          arg2_noise = arg2_it->second;
        }

        if (!is_cipher)
          throw logic_error("binary operation ciphertext node with two non-ciphertext operands");

        auto noise_growth_it =
          operations_noise_growth.find({node->get_opcode(), arg1->get_term_type(), arg2->get_term_type()});
        if (noise_growth_it == operations_noise_growth.end())
          throw logic_error("unhandled operation (opcode + operands types)");

        result_noise = max(arg1_noise, arg2_noise) + noise_growth_it->second;
      }
      else if (node->get_operands().size() == 1)
      {
        const auto &arg1 = node->get_operands()[0];
        int arg1_noise;
        if (arg1->get_term_type() != ir::TermType::ciphertext)
        {
          if (node->get_opcode() != ir::OpCode::encrypt)
            throw logic_error("unary operation ciphertext node (not encrypt) with non-ciphertext operand");

          arg1_noise = 0;
        }
        else
        {
          auto arg1_it = nodes_noise.find(arg1->get_label());
          if (arg1_it == nodes_noise.end())
            throw logic_error("parent handled before child");

          arg1_noise = arg1_it->second;
        }
        auto noise_growth_it =
          operations_noise_growth.find({node->get_opcode(), arg1->get_term_type(), ir::TermType::undefined});
        if (noise_growth_it == operations_noise_growth.end())
          throw logic_error("unhandled operation (opcode + operands types)");

        result_noise = arg1_noise + noise_growth_it->second;
      }

      nodes_noise.insert({node->get_label(), result_noise});
      if (circuit_noise < result_noise)
        circuit_noise = result_noise;
    }
  }
  return circuit_noise;
}

bool ParameterSelector::insert_mod_switch_bfv(
  const vector<int> &data_level_primes_sizes, unordered_map<string, int> &nodes_noise, int safety_margin)
{
  bool used_mod_switch = false;

  auto make_leveled_node_label = [](const string &node_label, int level) -> string {
    return node_label + "_level" + to_string(level);
  };

  struct LevelData
  {
    int justified_level;
    int lowest_level;
    string root_label;
  };

  unordered_map<string, LevelData> nodes_level_data;
  int L = data_level_primes_sizes.size() - 1;

  const auto &nodes = program_->get_dataflow_sorted_nodes(false);
  for (const auto &node : nodes)
  {
    if (node->get_term_type() != ir::TermType::ciphertext)
      continue;

    auto node_noise_it = nodes_noise.find(node->get_label());
    if (node_noise_it == nodes_noise.end())
      throw logic_error("node noise not handled in noise simulation");

    if (nodes_level_data.find(node->get_label()) != nodes_level_data.end())
      throw logic_error("repeated node in dataflow_sorted_nodes");

    if (!node->is_operation_node())
      nodes_level_data.insert({node->get_label(), {L, L, node->get_label()}});
    else
    {
      int operands_level = L;
      if (node->get_operands().size() == 2)
      {
        const auto &arg1 = node->get_operands()[0];
        auto arg1_level_data_it = nodes_level_data.find(arg1->get_label());

        const auto &arg2 = node->get_operands()[1];
        auto arg2_level_data_it = nodes_level_data.find(arg2->get_label());

        if (arg1->get_term_type() == ir::TermType::ciphertext && arg2->get_term_type() == ir::TermType::ciphertext)
        {
          if (arg1_level_data_it == nodes_level_data.end())
            throw logic_error("parent handled before child");
          LevelData &arg1_level_data = arg1_level_data_it->second;

          if (arg2_level_data_it == nodes_level_data.end())
            throw logic_error("parent handled before child");
          LevelData &arg2_level_data = arg2_level_data_it->second;

          if (arg1_level_data.justified_level > arg2_level_data.justified_level)
          {
            // node->delete_operand_term(make_leveled_node_label(arg1->get_label(), arg1_level_data.justified_level));
            ir::Term::Ptr arg1_matching_level;
            if (arg1_level_data.lowest_level > arg2_level_data.justified_level)
            {
              ir::Term::Ptr arg1_lowest_level;
              if (arg1_level_data.root_label != arg1->get_label())
              {
                arg1_lowest_level = program_->find_node_in_dataflow(
                  make_leveled_node_label(arg1_level_data.root_label, arg1_level_data.lowest_level));
                if (!arg1_lowest_level)
                  throw logic_error("node lowest level not found in the data flow");
              }
              else
                arg1_lowest_level = arg1;

              while (arg1_level_data.lowest_level > arg2_level_data.justified_level)
              {
                --arg1_level_data.lowest_level;
                ir::Term::Ptr mod_switch_node = program_->insert_operation_node_in_dataflow(
                  ir::OpCode::modswitch, vector<ir::Term::Ptr>({arg1_lowest_level}),
                  make_leveled_node_label(arg1_level_data.root_label, arg1_level_data.lowest_level),
                  ir::TermType::ciphertext);
                arg1_lowest_level->add_parent_label(mod_switch_node->get_label());
                arg1_lowest_level = mod_switch_node;
              }
              arg1_matching_level = arg1_lowest_level;
            }
            else
              arg1_matching_level = program_->find_node_in_dataflow(
                make_leveled_node_label(arg1_level_data.root_label, arg2_level_data.justified_level));
            if (!arg1_matching_level)
              throw logic_error("leveled version of node supposed to be present");

            size_t operand_index = node->delete_operand_term(arg1->get_label());
            if (operand_index < 0)
              throw logic_error("verified parent however could not delete child operand from parent");

            node->add_operand(arg1_matching_level, operand_index);
          }
          else if (arg1_level_data.justified_level < arg2_level_data.justified_level)
          {
            ir::Term::Ptr arg2_matching_level;
            if (arg2_level_data.lowest_level > arg1_level_data.justified_level)
            {
              ir::Term::Ptr arg2_lowest_level;
              if (arg2_level_data.root_label != arg2->get_label())
              {
                arg2_lowest_level = program_->find_node_in_dataflow(
                  make_leveled_node_label(arg2_level_data.root_label, arg2_level_data.lowest_level));
                if (!arg2_lowest_level)
                  throw logic_error("node lowest level not found in the data flow");
              }
              else
                arg2_lowest_level = arg2;

              while (arg2_level_data.lowest_level > arg1_level_data.justified_level)
              {
                --arg2_level_data.lowest_level;
                ir::Term::Ptr mod_switch_node = program_->insert_operation_node_in_dataflow(
                  ir::OpCode::modswitch, vector<ir::Term::Ptr>({arg2_lowest_level}),
                  make_leveled_node_label(arg2_level_data.root_label, arg2_level_data.lowest_level),
                  ir::TermType::ciphertext);
                arg2_lowest_level->add_parent_label(mod_switch_node->get_label());
                arg2_lowest_level = mod_switch_node;
              }
              arg2_matching_level = arg2_lowest_level;
            }
            else
              arg2_matching_level = program_->find_node_in_dataflow(
                make_leveled_node_label(arg2_level_data.root_label, arg1_level_data.justified_level));
            if (!arg2_matching_level)
              throw logic_error("leveled version of node supposed to be present");

            size_t operand_index = node->delete_operand_term(arg2->get_label());
            if (operand_index < 0)
              throw logic_error("verified parent however could not delete child operand from parent");

            node->add_operand(arg2_matching_level, operand_index);
          }
        }

        if (arg1->get_term_type() == ir::TermType::ciphertext)
        {
          if (arg1_level_data_it == nodes_level_data.end())
            throw logic_error("parent handled before child");

          operands_level = arg1_level_data_it->second.justified_level;
        }
        if (arg2->get_term_type() == ir::TermType::ciphertext)
        {
          if (arg2_level_data_it == nodes_level_data.end())
            throw logic_error("parent handled before child");

          operands_level = min(operands_level, arg2_level_data_it->second.justified_level);
        }
      }
      else if (node->get_operands().size() == 1)
      {
        const auto &arg1 = node->get_operands()[0];
        if (arg1->get_term_type() != ir::TermType::ciphertext)
        {
          if (node->get_opcode() != ir::OpCode::encrypt)
            throw logic_error("unary operation ciphertext node (not encrypt) with non-ciphertext operand");

          operands_level = L;
        }
        else
        {
          auto arg1_level_data_it = nodes_level_data.find(arg1->get_label());
          if (arg1_level_data_it == nodes_level_data.end())
            throw logic_error("parent handled before child");

          operands_level = arg1_level_data_it->second.justified_level;
        }
      }

      int node_level = operands_level;
      if (node_level < 0)
        throw logic_error("invalid node_level");

      int node_adapted_noise = node_noise_it->second;
      for (int i = L; i > node_level; --i)
        node_adapted_noise -= data_level_primes_sizes[i];

      auto node_old_parents = node->get_parents_labels();
      ir::Term::Ptr mod_switch_arg = node;
      while (node_level >= 0 && node_adapted_noise > data_level_primes_sizes[node_level] + safety_margin)
      {
        node_adapted_noise -= data_level_primes_sizes[node_level];
        --node_level;
        ir::Term::Ptr mod_switch_node = program_->insert_operation_node_in_dataflow(
          ir::OpCode::modswitch, vector<ir::Term::Ptr>({mod_switch_arg}),
          make_leveled_node_label(node->get_label(), node_level), ir::TermType::ciphertext);
        mod_switch_arg->insert_parent_label(mod_switch_node->get_label());
        mod_switch_arg = mod_switch_node;
      }

      if (node_level < 0)
        throw logic_error("invalid node_level");

      if (node_level < operands_level)
      {
        used_mod_switch = true;
        for (const auto &parent_label : node_old_parents)
        {
          ir::Term::Ptr parent = program_->find_node_in_dataflow(parent_label);
          if (!parent)
            throw logic_error("parent node not found");

          size_t operand_index = parent->delete_operand_term(node->get_label());
          if (operand_index < 0)
            throw logic_error("verified parent however could not delete child operand from parent");

          parent->add_operand(mod_switch_arg, operand_index);
        }
      }
      nodes_level_data.insert({mod_switch_arg->get_label(), {node_level, node_level, node->get_label()}});
    }
  }
  return used_mod_switch;
}

void apply_mod_switch_schedule(
  ir::Program *program, const unordered_map<string, tuple<size_t, size_t>> &nodes_noise,
  const unordered_map<string, unordered_map<string, size_t>> &nodes_level_matching_mod_switch)
{
  auto make_leveled_node_label = [](const string &node_label, size_t level) -> string {
    return node_label + "_level" + to_string(level);
  };

  for (const auto &node_noise : nodes_noise)
  {
    ir::Term::Ptr node = program->find_node_in_dataflow(node_noise.first);
    if (!node)
      throw logic_error(
        "node handled in noise simulation yet not found in the data flow, label: '" + node_noise.first + "'");

    ir::Term::Ptr mod_switch_arg = node;
    for (int i = 0; i < get<1>(node_noise.second); ++i)
    {
      ir::Term::Ptr mod_switch_node = program->insert_operation_node_in_dataflow(
        ir::OpCode::modswitch, vector<ir::Term::Ptr>({mod_switch_arg}), make_leveled_node_label(node_noise.first, i),
        ir::TermType::ciphertext);
      mod_switch_arg = mod_switch_node;
    }
    for (const auto &parent_label : node->get_parents_labels())
    {
      ir::Term::Ptr parent = program->find_node_in_dataflow(parent_label);
      if (!parent)
        throw logic_error("parent node not found");

      parent->delete_operand_term(node_noise.first);
      parent->add_operand(mod_switch_arg);
    }
  }

  for (const auto &node_level_matching_mod_switch : nodes_level_matching_mod_switch)
  {
    auto noise_it = nodes_noise.find(node_level_matching_mod_switch.first);
    if (noise_it == nodes_noise.end())
      throw logic_error("unknown ciphertext, no level");

    size_t justified_level = get<1>(noise_it->second);
    size_t highest_level = justified_level;
    for (const auto &mod_switch : node_level_matching_mod_switch.second)
    {
      ir::Term::Ptr parent = program->find_node_in_dataflow(mod_switch.first);
      if (!parent)
        throw logic_error(
          "node present as a parent in level matching map yet not found in the data flow, label: '" + mod_switch.first +
          "'");

      parent->delete_operand_term(make_leveled_node_label(node_level_matching_mod_switch.first, justified_level));

      if (justified_level + mod_switch.second <= highest_level)
      {
        ir::Term::Ptr new_child = program->find_node_in_dataflow(
          make_leveled_node_label(node_level_matching_mod_switch.first, justified_level + mod_switch.second));
        if (!new_child)
          throw logic_error(
            "leveled version of node supposed to be present, label: '" +
            make_leveled_node_label(node_level_matching_mod_switch.first, justified_level + mod_switch.second) + "'");

        parent->add_operand(new_child);
      }
      else
      {
        ir::Term::Ptr node_highest_level =
          program->find_node_in_dataflow(make_leveled_node_label(node_level_matching_mod_switch.first, highest_level));
        if (!node_highest_level)
          throw logic_error(
            "node_highest_level not found in the data flow, label: '" +
            make_leveled_node_label(node_level_matching_mod_switch.first, highest_level) + "'");

        while (highest_level < justified_level + mod_switch.second)
        {
          ir::Term::Ptr mod_switch_node = program->insert_operation_node_in_dataflow(
            ir::OpCode::modswitch, vector<ir::Term::Ptr>({node_highest_level}),
            make_leveled_node_label(node_level_matching_mod_switch.first, highest_level), ir::TermType::ciphertext);
          node_highest_level = mod_switch_node;
          ++highest_level;
        }
        parent->add_operand(node_highest_level);
      }
    }
  }
}

unordered_map<string, unordered_set<string>> ParameterSelector::get_outputs_composing_nodes() const
{
  unordered_map<string, unordered_set<string>> outputs_composing_nodes;

  const auto &nodes = program_->get_dataflow_sorted_nodes(false);
  for (const auto &node : nodes)
  {
    if (node->get_term_type() != ir::TermType::ciphertext)
      continue;

    if (node->is_operation_node())
    {
      auto [node_composing_nodes_it, node_composing_nodes_exists] =
        outputs_composing_nodes.insert({node->get_label(), {}});

      if (node_composing_nodes_exists)
        throw logic_error("repeated node in dataflow_sorted_nodes");

      if (node->get_operands().size() == 2)
      {
        const auto &arg1 = node->get_operands()[0];
        const auto &arg2 = node->get_operands()[1];

        if (arg1->get_term_type() == ir::TermType::ciphertext)
        {
          auto arg1_composing_nodes_it = outputs_composing_nodes.find(arg1->get_label());
          if (arg1_composing_nodes_it != outputs_composing_nodes.end())
          {
            node_composing_nodes_it->second.insert(
              arg1_composing_nodes_it->second.begin(), arg1_composing_nodes_it->second.end());
            outputs_composing_nodes.erase(arg1_composing_nodes_it);
          }
          node_composing_nodes_it->second.insert(arg1->get_label());
        }
        if (arg2->get_term_type() == ir::TermType::ciphertext)
        {
          auto arg2_composing_nodes_it = outputs_composing_nodes.find(arg2->get_label());
          if (arg2_composing_nodes_it != outputs_composing_nodes.end())
          {
            node_composing_nodes_it->second.insert(
              arg2_composing_nodes_it->second.begin(), arg2_composing_nodes_it->second.end());
            outputs_composing_nodes.erase(arg2_composing_nodes_it);
          }
          node_composing_nodes_it->second.insert(arg2->get_label());
        }
      }
      else if (node->get_operands().size() == 1)
      {
        const auto &arg1 = node->get_operands()[0];
        int arg1_noise;
        if (arg1->get_term_type() == ir::TermType::ciphertext)
        {
          auto arg1_composing_nodes_it = outputs_composing_nodes.find(arg1->get_label());
          if (arg1_composing_nodes_it != outputs_composing_nodes.end())
          {
            node_composing_nodes_it->second.insert(
              arg1_composing_nodes_it->second.begin(), arg1_composing_nodes_it->second.end());
            outputs_composing_nodes.erase(arg1_composing_nodes_it);
          }
          node_composing_nodes_it->second.insert(arg1->get_label());
        }
      }
    }
  }
  return outputs_composing_nodes;
}

} // namespace param_selector
