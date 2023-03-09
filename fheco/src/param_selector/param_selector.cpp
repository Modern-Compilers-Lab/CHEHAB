#include "param_selector.hpp"
#include "fhecompiler_const.hpp"
#include "term.hpp"
#include <map>
#include <stdexcept>

using namespace std;

namespace param_selector
{

unordered_map<fhecompiler::SecurityLevel, unordered_map<size_t, int>> ParameterSelector::security_standard = {
  {fhecompiler::SecurityLevel::tc128, {{1024, 27}, {2048, 54}, {4096, 109}, {8192, 218}, {16384, 438}, {32768, 881}}},
  {fhecompiler::SecurityLevel::tc192, {{1024, 19}, {2048, 37}, {4096, 75}, {8192, 152}, {16384, 305}, {32768, 612}}},
  {fhecompiler::SecurityLevel::tc256, {{1024, 14}, {2048, 29}, {4096, 58}, {8192, 118}, {16384, 237}, {32768, 476}}}};

vector<tuple<int, vector<tuple<size_t, tuple<int, int, int>>>>> ParameterSelector::bfv_noise_estimates_seal = {
  {17, {{1024, {8, 22, 30}}}}};

tuple<vector<int>, int> split_coeff_mod_seal(int data_level_size)
{
  vector<int> primes_sizes(data_level_size / MOD_BIT_COUNT_MAX, MOD_BIT_COUNT_MAX);
  if (data_level_size % MOD_BIT_COUNT_MAX)
  {
    primes_sizes.push_back(MOD_BIT_COUNT_MAX);
    // Remove exceeding bits
    for (int i = 0; i < MOD_BIT_COUNT_MAX - (data_level_size % MOD_BIT_COUNT_MAX); ++i)
      --primes_sizes[(primes_sizes.size() - 1 - i) % primes_sizes.size()];
  }
  // Add special prime
  primes_sizes.push_back(primes_sizes.back());
  return {primes_sizes, data_level_size + primes_sizes.back()};
}

int increase_coeff_mod_bit_sizes_seal(vector<int> &primes_sizes, int max_total_amount)
{
  auto last_big_prime_index = [](const vector<int> &primes_sizes) -> int {
    int idx = 0;
    // Consider only data level primes
    while (idx < primes_sizes.size() - 2 && primes_sizes[idx] == primes_sizes[idx + 1])
      ++idx;
    return idx;
  };

  int start_idx = last_big_prime_index(primes_sizes) + 1;
  int i;
  for (i = 0; i < max_total_amount; ++i)
  {
    int prime_idx = (start_idx + i) % primes_sizes.size();
    if (primes_sizes[prime_idx] == MOD_BIT_COUNT_MAX)
      break;

    ++primes_sizes[prime_idx];
  }
  return i;
}

vector<int> get_data_level_coeff_mod_seal(const vector<int> &primes_sizes)
{
  vector<int> data_level_part(primes_sizes);
  // Remove special prime
  data_level_part.pop_back();
  return data_level_part;
}

void ParameterSelector::select_params()
{
  program->set_params(select_params_bfv());
}

EncryptionParameters ParameterSelector::select_params_bfv(bool use_mod_switch)
{
  int plain_mod_size = program->get_bit_width() + 1;
  if (program->get_signedness())
    ++plain_mod_size;

  int idx = plain_mod_size - get<0>(bfv_noise_estimates_seal[0]);
  if (idx < 0 || idx > bfv_noise_estimates_seal.size())
    throw logic_error("missing noise estimates for plain_mod_size " + to_string(plain_mod_size));

  size_t poly_modulus_degree;
  vector<int> coeff_mod_bit_sizes;
  int coeff_mod_bit_count;
  unordered_map<string, int> nodes_noise;

  if (get<1>(bfv_noise_estimates_seal[idx]).size() == 0)
    throw logic_error("empty per poly_modulus_degree estimates vector for plain_mod_size " + to_string(plain_mod_size));

  for (const auto &n_noise_estimates : get<1>(bfv_noise_estimates_seal[idx]))
  {
    nodes_noise.clear();
    poly_modulus_degree = get<0>(n_noise_estimates);
    const auto &noise_estimates = get<1>(n_noise_estimates);
    int circuit_noise =
      simulate_noise_bfv(get<0>(noise_estimates), get<1>(noise_estimates), get<2>(noise_estimates), nodes_noise);

    int coeff_mod_data_level_size = plain_mod_size + circuit_noise;
    tie(coeff_mod_bit_sizes, coeff_mod_bit_count) = split_coeff_mod_seal(coeff_mod_data_level_size);

    if (program->get_sec_level() == fhecompiler::SecurityLevel::none)
      break;

    if (coeff_mod_bit_count <= security_standard[program->get_sec_level()][poly_modulus_degree])
      break;
  }
  int max_coeff_mod_bit_count = security_standard[program->get_sec_level()][poly_modulus_degree];
  int increase_amount =
    increase_coeff_mod_bit_sizes_seal(coeff_mod_bit_sizes, max_coeff_mod_bit_count - coeff_mod_bit_count);
  coeff_mod_bit_count += increase_amount;

  if (use_mod_switch)
    insert_mod_switch_bfv(get_data_level_coeff_mod_seal(coeff_mod_bit_sizes), nodes_noise);

  return EncryptionParameters(poly_modulus_degree, plain_mod_size, coeff_mod_bit_sizes, coeff_mod_bit_count);
}

int ParameterSelector::simulate_noise_bfv(
  int fresh_noise, int mul_noise_growth, int mul_plain_noise_growth, unordered_map<string, int> &nodes_noise) const
{
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
    {{ir::OpCode::rotate, ir::TermType::ciphertext, ir::TermType::rawData}, 0},
    {{ir::OpCode::rotate, ir::TermType::rawData, ir::TermType::ciphertext}, 0},
    {{ir::OpCode::assign, ir::TermType::ciphertext, ir::TermType::undefined}, 0},
    {{ir::OpCode::encrypt, ir::TermType::plaintext, ir::TermType::undefined}, fresh_noise}};

  int circuit_noise = fresh_noise;

  const auto &nodes = program->get_dataflow_sorted_nodes(true);
  for (const auto &node : nodes)
  {
    if (node->get_term_type() != ir::TermType::ciphertext)
      continue;

    auto it_node = nodes_noise.find(node->get_label());
    if (it_node != nodes_noise.end())
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
        auto arg1_it = nodes_noise.find(arg1->get_label());
        auto arg2_it = nodes_noise.find(arg2->get_label());

        int arg1_noise = 0;
        int arg2_noise = 0;
        bool is_cipher = false;
        if (arg1->get_term_type() == ir::TermType::ciphertext)
        {
          is_cipher = true;
          if (arg1_it == nodes_noise.end())
            throw logic_error("parent handled before child");
          arg1_noise = arg1_it->second;
        }
        if (arg2->get_term_type() == ir::TermType::ciphertext)
        {
          is_cipher = true;
          if (arg2_it == nodes_noise.end())
            throw logic_error("parent handled before child");
          arg2_noise = arg2_it->second;
        }

        if (!is_cipher)
          throw logic_error("binary operation ciphertext node with two non-ciphertext operands");

        auto noise_growth_it =
          operations_noise_growth.find({node->get_opcode(), arg1->get_term_type(), arg2->get_term_type()});
        if (noise_growth_it == operations_noise_growth.end())
          throw logic_error(
            "unhandled operation (opcode + operands types) " + to_string(static_cast<int>(node->get_opcode())) + " " +
            to_string(static_cast<int>(arg1->get_term_type())) + " " +
            to_string(static_cast<int>(arg2->get_term_type())));

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

void ParameterSelector::insert_mod_switch_bfv(
  vector<int> data_level_primes_sizes, unordered_map<string, int> &nodes_noise)
{}

void apply_mod_switch_schedule(
  ir::Program *program, const unordered_map<string, tuple<size_t, size_t>> &nodes_noise,
  const unordered_map<string, unordered_map<string, size_t>> &nodes_level_matching_mod_switch)
{
  auto make_leveled_node_label = [](const string &node_label, size_t level) -> string {
    return node_label + "_level" + to_string(level);
  };

  for (const auto &node_noise : nodes_noise)
  {
    ir::Program::Ptr node = program->find_node_in_dataflow(node_noise.first);
    if (node == nullptr)
      throw logic_error(
        "node handled in noise simulation yet not found in the data flow, label: '" + node_noise.first + "'");

    ir::Program::Ptr mod_switch_arg = node;
    for (int i = 0; i < get<1>(node_noise.second); ++i)
    {
      ir::Program::Ptr mod_switch_node = program->insert_operation_node_in_dataflow(
        ir::OpCode::modswitch, vector<ir::Program::Ptr>({mod_switch_arg}), make_leveled_node_label(node_noise.first, i),
        ir::TermType::ciphertext);
      mod_switch_arg = mod_switch_node;
    }
    for (const auto &parent_label : node->get_parents_labels())
    {
      ir::Program::Ptr parent = program->find_node_in_dataflow(parent_label);
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
      ir::Program::Ptr parent = program->find_node_in_dataflow(mod_switch.first);
      if (parent == nullptr)
        throw logic_error(
          "node present as a parent in level matching map yet not found in the data flow, label: '" + mod_switch.first +
          "'");

      parent->delete_operand_term(make_leveled_node_label(node_level_matching_mod_switch.first, justified_level));

      if (justified_level + mod_switch.second <= highest_level)
      {
        ir::Program::Ptr new_child = program->find_node_in_dataflow(
          make_leveled_node_label(node_level_matching_mod_switch.first, justified_level + mod_switch.second));
        if (new_child == nullptr)
          throw logic_error(
            "leveled version of node supposed to be present, label: '" +
            make_leveled_node_label(node_level_matching_mod_switch.first, justified_level + mod_switch.second) + "'");

        parent->add_operand(new_child);
      }
      else
      {
        ir::Program::Ptr node_highest_level =
          program->find_node_in_dataflow(make_leveled_node_label(node_level_matching_mod_switch.first, highest_level));
        if (node_highest_level == nullptr)
          throw logic_error(
            "node_highest_level not found in the data flow, label: '" +
            make_leveled_node_label(node_level_matching_mod_switch.first, highest_level) + "'");

        while (highest_level < justified_level + mod_switch.second)
        {
          ir::Program::Ptr mod_switch_node = program->insert_operation_node_in_dataflow(
            ir::OpCode::modswitch, vector<ir::Program::Ptr>({node_highest_level}),
            make_leveled_node_label(node_level_matching_mod_switch.first, highest_level), ir::TermType::ciphertext);
          node_highest_level = mod_switch_node;
          ++highest_level;
        }
        parent->add_operand(node_highest_level);
      }
    }
  }
}

} // namespace param_selector
