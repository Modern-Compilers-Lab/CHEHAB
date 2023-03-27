#include "param_selector.hpp"
#include "encryption_parameters.hpp"
#include "fhecompiler_const.hpp"
#include "term.hpp"
#include <stdexcept>

using namespace std;

namespace param_selector
{

unordered_map<fhecompiler::SecurityLevel, unordered_map<size_t, int>> ParameterSelector::security_standard = {
  {fhecompiler::SecurityLevel::tc128, {{1024, 27}, {2048, 54}, {4096, 109}, {8192, 218}, {16384, 438}, {32768, 881}}},
  {fhecompiler::SecurityLevel::tc192, {{1024, 19}, {2048, 37}, {4096, 75}, {8192, 152}, {16384, 305}, {32768, 612}}},
  {fhecompiler::SecurityLevel::tc256, {{1024, 14}, {2048, 29}, {4096, 58}, {8192, 118}, {16384, 237}, {32768, 476}}}};

map<int, map<size_t, ParameterSelector::NoiseEstimatesValue>> ParameterSelector::bfv_noise_estimates_seal = {
  {17, {{1024, {8, 30, 22}}, {2048, {8, 31, 23}}, {4096, {8, 32, 24}}, {8192, {8, 33, 25}}}}};

void ParameterSelector::select_params()
{
  switch (program_->get_scheme())
  {
  case fhecompiler::Scheme::bfv:
    select_params_bfv();
    break;

  case fhecompiler::Scheme::none:
    throw logic_error("no shceme was specified");

  default:
    throw logic_error("parameter selection unsupported for the scheme");
    break;
  }
}

void ParameterSelector::select_params_bfv(bool use_mod_switch)
{
  int plain_mod_size = program_->get_bit_width() + 1;
  if (program_->get_signedness())
    ++plain_mod_size;

  auto plain_mod_noise_estimates_it = bfv_noise_estimates_seal.find(plain_mod_size);
  while (plain_mod_noise_estimates_it == bfv_noise_estimates_seal.end() &&
         plain_mod_size < bfv_noise_estimates_seal.rend()->first && plain_mod_size < MOD_BIT_COUNT_MAX)
  {
    ++plain_mod_size;
    plain_mod_noise_estimates_it = bfv_noise_estimates_seal.find(plain_mod_size);
  }
  if (plain_mod_noise_estimates_it == bfv_noise_estimates_seal.end())
    throw logic_error("noise estimates maximum plaintext modulus size smaller than bit_width+1+signedness");

  if (plain_mod_noise_estimates_it->second.empty())
    throw logic_error("empty per polynomial modulus degree estimates map for the given plaintext modulus size");

  size_t poly_modulus_degree = program_->get_vector_size();
  auto poly_modulus_degree_noise_estimates_it = plain_mod_noise_estimates_it->second.find(poly_modulus_degree);

  while (poly_modulus_degree_noise_estimates_it == plain_mod_noise_estimates_it->second.end() &&
         poly_modulus_degree < plain_mod_noise_estimates_it->second.rend()->first)
  {
    poly_modulus_degree = poly_modulus_degree << 1;
    poly_modulus_degree_noise_estimates_it = plain_mod_noise_estimates_it->second.find(poly_modulus_degree);
  }
  if (poly_modulus_degree_noise_estimates_it == plain_mod_noise_estimates_it->second.end())
    throw logic_error("the maximum polynomial modulus degree of the noise estimates for the given plaintext modulus "
                      "size is smaller than vector_size");

  EncryptionParameters params;
  unordered_map<string, int> nodes_noise;
  while (poly_modulus_degree_noise_estimates_it != plain_mod_noise_estimates_it->second.end())
  {
    poly_modulus_degree = poly_modulus_degree_noise_estimates_it->first;
    const NoiseEstimatesValue &noise_estimates_value = poly_modulus_degree_noise_estimates_it->second;
    nodes_noise.clear();
    int circuit_noise = simulate_noise_bfv(noise_estimates_value, nodes_noise);

    int coeff_mod_data_level_size = plain_mod_size + circuit_noise;
    params = EncryptionParameters(poly_modulus_degree, plain_mod_size, coeff_mod_data_level_size);

    if (program_->get_sec_level() == fhecompiler::SecurityLevel::none)
      break;

    auto sec_level_he_standard_it = security_standard.find(program_->get_sec_level());
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

  if (program_->get_sec_level() != fhecompiler::SecurityLevel::none)
  {
    int max_coeff_mod_bit_count = security_standard[program_->get_sec_level()][poly_modulus_degree];
    params.increase_coeff_mod_bit_sizes(max_coeff_mod_bit_count - params.coeff_mod_bit_count());
  }
  else
    params.increase_coeff_mod_bit_sizes(MOD_BIT_COUNT_MAX);

  if (use_mod_switch)
    insert_mod_switch_bfv(params.coeff_mod_data_level_bit_sizes(), nodes_noise);

  program_->set_params(params);
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
    {{ir::OpCode::rotate, ir::TermType::ciphertext, ir::TermType::rawData}, 0},
    {{ir::OpCode::rotate, ir::TermType::rawData, ir::TermType::ciphertext}, 0},
    {{ir::OpCode::assign, ir::TermType::ciphertext, ir::TermType::undefined}, 0},
    {{ir::OpCode::encrypt, ir::TermType::plaintext, ir::TermType::undefined}, fresh_noise}};

  int circuit_noise = fresh_noise;

  const auto &nodes = program_->get_dataflow_sorted_nodes(true);
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
