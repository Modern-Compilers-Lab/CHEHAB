#include "rotationkeys_select_pass.hpp"
#include "passes_const.hpp"
#include <algorithm>

namespace fheco_passes
{

/*
  This is just an implementation of NAF, we got implementation below from
  (https://github.com/microsoft/SEAL/blob/82b07db635132e297282649e2ab5908999089ad2/native/src/seal/util/numth.h#L22)
*/
std::vector<int> RotationKeySelctionPass::naf(int value)
{
  std::vector<int> res;

  // Record the sign of the original value and compute abs
  bool sign = value < 0;
  value = std::abs(value);

  // Transform to non-adjacent form (NAF)
  for (int i = 0; value; i++)
  {
    int zi = (value & int(0x1)) ? 2 - (value & int(0x3)) : 0;
    value = (value - zi) >> 1;
    if (zi)
    {
      res.push_back((sign ? -zi : zi) * (1 << i));
    }
  }
  return res;
}

std::vector<int> RotationKeySelctionPass::get_unique_rotation_steps()
{

  std::unordered_map<int32_t, int32_t> rotation_steps;
  /* rotate_columns is not included in rotation_intruction_set since this instruction doesn't take steps as input */

  auto sorted_nodes = program->get_dataflow_sorted_nodes(true);
  for (auto &node : sorted_nodes)
  {
    if (node->is_operation_node() == false)
      continue;

    if (rotation_intruction_set.find(node->get_opcode()) == rotation_intruction_set.end())
      continue;

    auto &operands = node->get_operands();
    std::string steps_raw_data;
    if (operands.size() != 2)
      throw("rotation instruction with more than 2 operands in get_unique_rotation_steps function");

    if (operands[0]->get_term_type() == ir::rawDataType)
    {
      steps_raw_data = operands[0]->get_label();
    }
    else if (operands[1]->get_term_type() == ir::rawDataType)
    {
      steps_raw_data = operands[1]->get_label();
    }
    else
      throw("one of rotation operands must be a rawDataType in get_unique_rotation_steps function");

    rotation_steps[std::stoi(steps_raw_data)]++;
  }

  std::vector<int> rotations_steps_vec;
  for (auto &e : rotation_steps)
    rotations_steps_vec.push_back(e.first);

  if (rotation_steps.size() <= number_of_rotation_keys_threshold)
  {
    return rotations_steps_vec;
  }
  else
  {
    // we generate all necessary powers of 2 alongs with a given number S of additional non power of 2 steps
    auto is_power_of_two = [](int32_t value) -> bool {
      return __builtin_popcount(value) == 1;
    };

    std::unordered_map<int32_t, std::vector<int32_t>> naf_cache;
    for (auto &x : rotations_steps_vec)
      naf_cache[x] = naf(x);

    std::unordered_map<int32_t, int32_t> loss_by_step;
    for (auto &e : rotation_steps)
      loss_by_step[e.first] = e.second * (naf_cache[e.first].size() - 1);

    std::sort(rotations_steps_vec.begin(), rotations_steps_vec.end(), [&loss_by_step](int32_t lhs, int32_t rhs) {
      return loss_by_step[lhs] > loss_by_step[rhs];
    });

    std::unordered_set<int32_t> steps_to_rewrite;
    std::unordered_map<int32_t, int32_t> powers_of_2_steps;
    // eviction and rewrite of rotation steps
    int32_t total_number_of_keys = rotations_steps_vec.size();
    while (total_number_of_keys > number_of_rotation_keys_threshold)
    {
      int32_t key_with_minimal_loss = rotations_steps_vec.back();
      steps_to_rewrite.insert(key_with_minimal_loss);
      rotations_steps_vec.pop_back();
      total_number_of_keys -= 1;
      for (auto &power_of_2_step : naf_cache[key_with_minimal_loss])
      {
        if (powers_of_2_steps.find(power_of_2_step) == powers_of_2_steps.end())
          total_number_of_keys += 1;

        powers_of_2_steps[power_of_2_step]++;
      }
    }

    // normalize to increase cse chances
    for (auto &step : steps_to_rewrite)
      std::sort(naf_cache[step].begin(), naf_cache[step].end(), std::greater<int32_t>());

    for (auto &e : powers_of_2_steps)
      rotations_steps_vec.push_back(e.first);

    // we do a rewrite pass for rotation steps in steps_to_rewrite
    for (auto &node : sorted_nodes)
    {
      if (node->is_operation_node() == false)
        continue;

      if (rotation_intruction_set.find(node->get_opcode()) == rotation_intruction_set.end())
        continue;

      auto &operands = node->get_operands();
      std::string steps_raw_data;
      if (operands.size() != 2)
        throw("rotation instruction with more than 2 operands in get_unique_rotation_steps function");

      if (operands[0]->get_term_type() == ir::rawDataType)
      {
        steps_raw_data = operands[0]->get_label();
      }
      else if (operands[1]->get_term_type() == ir::rawDataType)
      {
        steps_raw_data = operands[1]->get_label();
      }
      else
        throw("one of rotation operands must be a rawDataType in get_unique_rotation_steps function");

      int32_t rotation_step = std::stoi(steps_raw_data);

      if (steps_to_rewrite.find(rotation_step) != steps_to_rewrite.end())
      {
        /*
        this rotation step needs to be written in naf because it doesn't exists in rotations_step_vec while its naf
        components are there
        */
        std::vector<int32_t> naf_components = naf_cache[rotation_step];
        rewrite_rotation_node_with_naf(node, naf_components);
      }
    }

    return rotations_steps_vec;
  }
}

void RotationKeySelctionPass::rewrite_rotation_node_with_naf(
  const ir::Program::Ptr &node, const std::vector<int32_t> naf_components)
{
  ir::Program::Ptr input_node;
  if (node->get_operands()[0]->get_term_type() == ir::ciphertextType)
    input_node = node->get_operands()[0];
  else
    input_node = node->get_operands()[1];

  for (size_t i = 0; i < naf_components.size(); i++)
  {

    std::string rotation_step = std::to_string(naf_components[i]);
    ir::Program::Ptr rotation_step_node = std::make_shared<ir::Term>(rotation_step, ir::TermType::rawDataType);
    ir::Program::Ptr node_to_rewrite_with = program->insert_operation_node_in_dataflow(
      node->get_opcode(), std::vector<ir::Program::Ptr>({input_node, rotation_step_node}), "", ir::ciphertextType);
    node_to_rewrite_with->set_a_default_label();

    if (i == naf_components.size() - 1)
    {
      node->clear_operands();
      node->set_operands(node_to_rewrite_with->get_operands());
    }
    else
      input_node = node_to_rewrite_with;
  }
}

} // namespace fheco_passes
