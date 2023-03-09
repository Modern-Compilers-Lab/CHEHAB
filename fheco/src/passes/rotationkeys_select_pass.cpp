#include "rotationkeys_select_pass.hpp"
#include "ir_utils.hpp"
#include "passes_const.hpp"
#include <algorithm>
#include <set>

namespace fheco_passes
{

inline std::string print_naf(const std::vector<int> &naf_rep)
{
  std::string result = "";
  for (size_t i = 0; i < naf_rep.size(); i++)
  {
    result += std::to_string(naf_rep[i]);
    if (i < naf_rep.size() - 1 && naf_rep[i + 1] >= 0)
      result += "+";
  }
  return result;
}

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

void RotationKeySelctionPass::decompose_rotations()
{

  auto is_power_of_two = [](int value) -> bool {
    return __builtin_popcount(value) == 1;
  };

  std::unordered_map<int, int> rotation_steps;
  /* rotate_columns is not included in rotation_intruction_set since this instruction doesn't take steps as input */

  auto sorted_nodes = program->get_dataflow_sorted_nodes(true);

  std::vector<ir::Program::Ptr> targeted_rotation_nodes;

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

    if (operands[0]->get_term_type() == ir::TermType::rawData)
    {
      steps_raw_data = operands[0]->get_label();
    }
    else if (operands[1]->get_term_type() == ir::TermType::rawData)
    {
      steps_raw_data = operands[1]->get_label();
    }
    else
      throw("one of rotation operands must be a rawDataType in get_unique_rotation_steps function");

    rotation_steps[std::stoi(steps_raw_data)]++;
    if (is_power_of_two(std::stoi(steps_raw_data)) == false)
      targeted_rotation_nodes.push_back(node);
  }

  std::vector<int> rotations_steps_vec;
  for (auto &e : rotation_steps)
    rotations_steps_vec.push_back(e.first);

  if (rotation_steps.size() <= number_of_rotation_keys_threshold)
  {
    program->set_rotations_keys_steps(std::set(rotations_steps_vec.begin(), rotations_steps_vec.end()));
    return;
  }
  else
  {
    // we generate all necessary powers of 2 alongs with a given number S of additional non power of 2 steps
    std::unordered_map<int, std::vector<int>> naf_cache;
    for (auto &x : rotations_steps_vec)
      naf_cache[x] = naf(x);

    std::unordered_map<int, int> loss_by_step;
    for (auto &e : rotation_steps)
      loss_by_step[e.first] = e.second * (naf_cache[e.first].size() - 1);

    std::sort(rotations_steps_vec.begin(), rotations_steps_vec.end(), [&loss_by_step, &naf_cache](int lhs, int rhs) {
      if (loss_by_step[lhs] == loss_by_step[rhs])
        return naf_cache[lhs].size() < naf_cache[rhs].size();
      else
        return loss_by_step[lhs] > loss_by_step[rhs];
    });

    std::unordered_set<int> steps_to_rewrite;
    std::unordered_set<int> powers_of_2_steps;
    // eviction and rewrite of rotation steps
    int total_number_of_keys = rotations_steps_vec.size();
    while (total_number_of_keys > number_of_rotation_keys_threshold)
    {
      int key_with_minimal_loss = rotations_steps_vec.back();

      if (is_power_of_two(key_with_minimal_loss) == false)
        steps_to_rewrite.insert(key_with_minimal_loss);

      rotations_steps_vec.pop_back();
      total_number_of_keys -= 1;

      for (auto &power_of_2_step : naf_cache[key_with_minimal_loss])
      {
        if (powers_of_2_steps.find(power_of_2_step) == powers_of_2_steps.end())
          total_number_of_keys += 1;

        powers_of_2_steps.insert(power_of_2_step);
      }
    }

    // sort in descending order to increase chances of CSE, this greedy decision was made based on observation
    for (auto &step : steps_to_rewrite)
      std::sort(naf_cache[step].begin(), naf_cache[step].end(), std::greater<int>());

    for (auto &step : powers_of_2_steps)
      rotations_steps_vec.push_back(step);

    // we do a rewrite pass for rotation steps in steps_to_rewrite
    for (auto &node : targeted_rotation_nodes)
    {
      auto &operands = node->get_operands();
      int rotation_step = ir::get_rotation_step(node);

      if (steps_to_rewrite.find(rotation_step) != steps_to_rewrite.end())
      {
        /*
          this rotation step needs to be written in naf because it doesn't exists in rotations_step_vec while its naf
          components are there
        */
        std::vector<int> naf_components = naf_cache[rotation_step];
        rewrite_rotation_node_with_naf(node, naf_components);
      }
    }
    program->set_rotations_keys_steps(std::set(rotations_steps_vec.begin(), rotations_steps_vec.end()));

    return;
  }
}

void RotationKeySelctionPass::rewrite_rotation_node_with_naf(
  const ir::Program::Ptr &node, const std::vector<int> naf_components)
{

  ir::Program::Ptr input_node;
  if (node->get_operands()[0]->get_term_type() == ir::TermType::ciphertext)
    input_node = node->get_operands()[0];
  else
    input_node = node->get_operands()[1];

  for (size_t i = 0; i < naf_components.size(); i++)
  {
    int rotation_step = naf_components[i]; // a power of 2 rotation step
    if (std::abs(rotation_step) == program->get_vector_size())
      continue;

    std::string rotation_step_raw = std::to_string(rotation_step);
    ir::Program::Ptr rotation_step_node = std::make_shared<ir::Term>(rotation_step_raw, ir::TermType::rawData);
    ir::Program::Ptr node_to_rewrite_with = program->insert_operation_node_in_dataflow(
      node->get_opcode(), std::vector<ir::Program::Ptr>({input_node, rotation_step_node}), "",
      ir::TermType::ciphertext);

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
