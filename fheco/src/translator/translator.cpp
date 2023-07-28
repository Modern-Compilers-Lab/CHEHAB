#include "translator.hpp"
#include "cse_pass.hpp"
#include "fhecompiler_const.hpp"
#include "ir_const.hpp"
#include "program.hpp"
#include "rotationkeys_select_pass.hpp"
#include <fstream>

namespace translator
{

size_t Translator::scope_id = 0;

std::string Translator::get_identifier(const Ptr &term_ptr)
{

  if (program->type_of(term_ptr->get_label()) == ir::ConstantTableEntryType::input)
  {
    return get_input_identifier(term_ptr);
  }

  bool is_output = program->get_outputs_nodes().find(term_ptr->get_label()) != program->get_outputs_nodes().end();

  is_output = is_output || program->type_of(term_ptr->get_label()) == ir::ConstantTableEntryType::output;

  if (is_output)
  {
    return get_output_identifier(term_ptr->get_label(), term_ptr->get_term_type());
  }

  auto curr_term_ptr = program->find_node_in_dataflow(current_label[term_ptr]);

  if (generated_shareds.find(curr_term_ptr) != generated_shareds.end())
    return get_shared_identifier(curr_term_ptr);

  if (program->get_entry_form_constants_table(current_label[term_ptr]) != std::nullopt)
  {
    ir::ConstantTableEntry table_entry = *(program->get_entry_form_constants_table(current_label[term_ptr]));
    ir::ConstantTableEntry::EntryValue entry_value = table_entry.get_entry_value();
    return (entry_value.get_tag().length() == 0 ? current_label[term_ptr] : entry_value.get_tag());
  }
  else
    return current_label[term_ptr];
}

std::string Translator::get_identifier(const std::string &label)
{
  if (program->get_entry_form_constants_table(label) != std::nullopt)
  {
    ir::ConstantTableEntry table_entry = *(program->get_entry_form_constants_table(label));
    ir::ConstantTableEntry::EntryValue entry_value = table_entry.get_entry_value();
    return (entry_value.get_tag().length() == 0 ? label : entry_value.get_tag());
  }
  else
    return label;
}

std::string Translator::get_shared_identifier(const Ptr &term)
{
  std::string ident = get_identifier(current_label[term]);

  if (term->get_term_type() == ir::TermType::ciphertextType)
  {
    return shared_ciphers_map_id + "[\"" + ident + "\"]";
  }
  else
  {
    return shared_plains_map_id + "[\"" + ident + "\"]";
  }
}

std::string Translator::get_output_identifier(const std::string &output_label, ir::TermType term_type, bool from_map)
{
  std::string tag;
  if (program->get_entry_form_constants_table(output_label) != std::nullopt)
  {
    ir::ConstantTableEntry table_entry = *(program->get_entry_form_constants_table(output_label));
    ir::ConstantTableEntry::EntryValue entry_value = table_entry.get_entry_value();
    tag = entry_value.get_tag();
  }
  else
    tag = output_label;

  if (from_map == false)
    return tag;

  return std::string(outputs_class_identifier[term_type]) + "[\"" + tag + "\"]";
}

void Translator::translate_constant_table_entry(
  const ir::Program::Ptr &term, ir::ConstantTableEntry &table_entry, ir::TermType term_type, std::ofstream &os,
  bool is_new_object)
{
  /*
    basically in this function we need to generate a decalartion or a definition in C++ for a given entry.
    the entry could be representing an input, output, a constant (with raw data) or some temp variable with a tag
    specified by the programmer
  */

  // Retrieving needed information
  ir::ConstantTableEntry::EntryValue entry_value = table_entry.get_entry_value();
  ir::ConstantTableEntryType entry_type = table_entry.get_entry_type();
  std::string tag = get_identifier(term);
  // getting type
  std::string type_str;

  if (term_type == ir::TermType::scalarType)
  {
    fhecompiler::Scheme scheme = program->get_encryption_scheme();
    if (scheme == fhecompiler::bfv || scheme == fhecompiler::bgv)
      type_str = scalar_int;
    else if (scheme == fhecompiler::ckks)
    {
      type_str = scalar_float;
    }
    else
      throw("scheme not supported\n");
  }
  else
    type_str = types_map[term_type];

  if (entry_type == ir::ConstantTableEntryType::constant || entry_value.value != std::nullopt)
  {
    if (term_type == ir::plaintextType)
    {
      // encoding
      using VectorValue = ir::VectorValue;
      VectorValue vector_value = std::get<VectorValue>(*(entry_value.value));
      using VectorInt = std::vector<int64_t>;
      using VectorFloat = std::vector<double>;

      type_str = (program->get_encryption_scheme() == fhecompiler::ckks ? scalar_float : scalar_int);

      if (auto _vector_value = std::get_if<VectorInt>(&vector_value))
        encoding_writer.write_vector_encoding(
          os, tag, term->get_label(), *_vector_value, type_str,
          program->get_encryption_scheme() == fhecompiler::Scheme::ckks ? program->get_scale() : 0.0, is_new_object);
      else if (auto vector_literal = std::get_if<VectorFloat>(&vector_value))
        encoding_writer.write_vector_encoding(
          os, tag, term->get_label(), *_vector_value, type_str, program->get_scale(), is_new_object);
      else
        throw("unsupported data type by schemes\n");
    }

    else if (term_type == ir::scalarType)
    {
      // don't forget to reduce
      using ScalarValue = ir::ScalarValue;
      ScalarValue scalar_value = std::get<ScalarValue>(*(entry_value.value));

      if (auto value = std::get_if<int64_t>(&scalar_value))
      {
        int64_t casted_value = static_cast<int64_t>(*value);
        encoding_writer.write_scalar_encoding(
          os, tag, term->get_label(), std::to_string(casted_value), type_str,
          std::to_string(program->get_vector_size()),
          program->get_encryption_scheme() == fhecompiler::Scheme::ckks ? program->get_scale() : 0.0);
      }
      else
      {
        double e_value = std::get<double>(scalar_value);
        encoding_writer.write_scalar_encoding(
          os, tag, term->get_label(), std::to_string(e_value), type_str, std::to_string(program->get_vector_size()),
          program->get_scale());
      }
    }
    else
      throw("type not supported ");
  }
}

void Translator::translate_binary_operation(const Ptr &term_ptr, std::ofstream &os, size_t scope_id, bool is_new_object)
{
  std::string other_args(""); // this depends on the operation
  auto it = get_other_args_by_opcode.find(term_ptr->get_opcode());

  if (it != get_other_args_by_opcode.end())
    other_args = it->second;

  std::string op_identifier = get_identifier(term_ptr);
  auto &operands = term_ptr->get_operands();
  std::string lhs_identifier = get_identifier(operands[0]);
  std::string rhs_identifier = get_identifier(operands[1]);

  if (lhs_identifier.empty() || rhs_identifier.empty())
    throw("empty label operand");

  evaluation_writer.write_binary_operation(
    os, deduce_opcode_to_generate(term_ptr), op_identifier, lhs_identifier, rhs_identifier, term_ptr->get_term_type(),
    is_new_object);
}

void Translator::translate_nary_operation(const Ptr &term_ptr, std::ofstream &os, size_t scope_id)
{
  std::cout << "translation of nary \n";
}

void Translator::translate_unary_operation(const Ptr &term_ptr, std::ofstream &os, size_t scope_id, bool is_new_object)
{
  std::string op_identifier = get_identifier(term_ptr);

  std::string rhs_identifier = get_identifier(term_ptr->get_operands()[0]);
  // os << op_type << " " << op_identifier << ops_map[term_ptr->get_opcode()] << rhs_identifier << end_of_command <<
  // '\n';
  if (term_ptr->get_opcode() == ir::OpCode::assign)
  {
    write_assign_operation(os, op_identifier, rhs_identifier, term_ptr->get_term_type());
  }
  else
  {
    evaluation_writer.write_unary_operation(
      os, term_ptr->get_opcode(), op_identifier, rhs_identifier, term_ptr->get_term_type(), is_new_object);
  }
}

void Translator::translate_term(const Ptr &term, std::ofstream &os, size_t term_scope_id)
{
  auto constant_table_entry_opt = program->get_entry_form_constants_table(term->get_label());

  bool is_input = program->type_of(term->get_label()) == ir::ConstantTableEntryType::input;

  bool is_output = program->get_outputs_nodes().find(term->get_label()) != program->get_outputs_nodes().end();

  bool is_new_object(true);

  if (is_input == false && is_output == false)
  {
    if (is_shared_obj(term, term_scope_id))
    {
      if (free_global_objs[term->get_term_type()].size() > 0)
      {
        current_label[term] = free_global_objs[term->get_term_type()].back();
        free_global_objs[term->get_term_type()].pop_back();
        is_new_object = false;
      }
    }
    else
    {
      if (free_local_objs[term->get_term_type()][term_scope_id].size() > 0)
      {
        current_label[term] = free_local_objs[term->get_term_type()][term_scope_id].back();
        free_local_objs[term->get_term_type()][term_scope_id].pop_back();
        is_new_object = false;
      }
    }
  }
  else
    is_new_object = false;

  // we need to tranlsate the operation node
  if (term->is_operation_node())
  {
    auto &operands = term->get_operands();
    if (operands.size() == 0)
    {
      throw("unexpected size of operands, 0 operands");
    }

    if (operands.size() == 1)
    {
      if (term->get_opcode() == ir::OpCode::encrypt)
      {
        const std::string &plaintext_id = get_identifier(term->get_operands()[0]);

        const std::string &destination_cipher = get_identifier(term);

        encryption_writer.write_encryption(os, plaintext_id, destination_cipher, is_new_object);
      }
      else
      {
        translate_unary_operation(term, os, term_scope_id, is_new_object);
      }
    }
    else if (operands.size() == 2)
    {
      translate_binary_operation(term, os, term_scope_id, is_new_object);
    }
    else
    {
      translate_nary_operation(term, os, term_scope_id);
    }
  }
  else if (constant_table_entry_opt != std::nullopt)
  {
    ir::ConstantTableEntry &constant_table_entry = *constant_table_entry_opt;
    translate_constant_table_entry(term, constant_table_entry, term->get_term_type(), os, is_new_object);
  }
  if (is_new_object && is_shared_obj(term, term_scope_id))
  {
    generated_shareds.insert(term);
    write_as_shared_object(term, os);
  }
}

void Translator::translate_program(size_t threshold, std::ofstream &os_header, std::ofstream &os_source)
{
  auto &nodes = program->get_dataflow_sorted_nodes(true);
  for (auto &node : nodes)
  {
    current_label[node] = node->get_label();
    parents_copy[node] = node->get_parents_labels();
  }

  os_header << "#pragma once\n #include<unordered_map>\n #include<vector>\n #include\"seal/seal.h\"\n";

  os_source << "#include \"" << generated_class_name << ".hpp"
            << "\"\n";

  os_header << gen_steps_function_signature << ";\n";

  if (program->get_rotations_steps().size())
    write_rotations_steps_getter_def(program->get_rotations_steps(), os_source);

  make_scopes_graph(threshold);

  fix_ir_instructions_pass();

  generate_computation_class(os_header, os_source);
}

void Translator::write_scope_function_call(size_t scope_id, std::ofstream &os) const
{
  generate_function_call_class(os, "func_" + std::to_string(scope_id));
}

void Translator::generate_function_signature(std::ofstream &os) const
{
  ArgumentList argument_list;
  os << "void " << program->get_program_tag()
     << argument_list(
          {{encrypted_inputs_class_literal, inputs_class_identifier[ir::ciphertextType], AccessType::readAndModify},
           {encoded_inputs_class_literal, inputs_class_identifier[ir::plaintextType], AccessType::readAndModify},
           {encrypted_outputs_class_literal, outputs_class_identifier[ir::ciphertextType], AccessType::readAndModify},
           {encoded_outputs_class_literal, outputs_class_identifier[ir::plaintextType], AccessType::readAndModify},
           {context_type_literal, context_identifier, AccessType::readOnly},
           {relin_keys_type_literal, relin_keys_identifier, AccessType::readOnly},
           {galois_keys_type_literal, galois_keys_identifier, AccessType::readOnly},
           {public_key_literal, public_key_identifier, AccessType::readOnly}})
     << '\n';
}

void Translator::generate_function_signature_without_type(std::ofstream &os, const std::string &func_name) const
{
  ArgumentList argument_list;
  os << func_name
     << argument_list(
          {{encrypted_inputs_class_literal, inputs_class_identifier[ir::ciphertextType], AccessType::readAndModify},
           {encoded_inputs_class_literal, inputs_class_identifier[ir::plaintextType], AccessType::readAndModify},
           {encrypted_outputs_class_literal, outputs_class_identifier[ir::ciphertextType], AccessType::readAndModify},
           {encoded_outputs_class_literal, outputs_class_identifier[ir::plaintextType], AccessType::readAndModify},
           {context_type_literal, context_identifier, AccessType::readOnly},
           {relin_keys_type_literal, relin_keys_identifier, AccessType::readOnly},
           {galois_keys_type_literal, galois_keys_identifier, AccessType::readOnly},
           {public_key_literal, public_key_identifier, AccessType::readOnly}});
}

void Translator::write_input(const std::string &input_identifier, ir::TermType type, std::ostream &os)
{
  // retrieve an input from object
  os << types_map[type] << " " << input_identifier << " = " << inputs_class_identifier[type] << "["
     << stringfy_string(input_identifier) << "];" << '\n';
}

void Translator::write_output(
  const std::string &output_tag, const std::string &output_identifier, ir::TermType type, std::ostream &os)
{
  // insert output in object
  os << outputs_class_identifier[type] << "." << insert_object_instruction << "({" << stringfy_string(output_tag) << ","
     << output_identifier << "})"
     << ";" << '\n';
}

void Translator::write_assign_operation(
  std::ofstream &os, const std::string &lhs_id, const std::string &rhs_id, ir::TermType type)
{
  os << types_map[type] << " " << lhs_id << " = " << rhs_id << ";" << '\n';
}

void Translator::generate_key_generator(std::ofstream &os) const
{
  os << key_generator_type_literal << " " << key_generator_identifier << "(" << context_identifier << ");\n";
}

ir::OpCode Translator::deduce_opcode_to_generate(const Ptr &node) const
{
  if (node->is_operation_node() == false)
    return node->get_opcode();

  if (node->get_operands().size() == 1)
    return node->get_opcode();

  if (node->get_operands().size() == 2)
  {
    auto lhs = node->get_operands()[0];
    auto rhs = node->get_operands()[1];

    if (lhs->get_term_type() == rhs->get_term_type())
      return node->get_opcode();

    else if (
      node->get_term_type() == ir::ciphertextType &&
      (lhs->get_term_type() != ir::ciphertextType || rhs->get_term_type() != ir::ciphertextType))
    {
      switch (node->get_opcode())
      {
      case ir::OpCode::add:
        return ir::OpCode::add_plain;
        break;

      case ir::OpCode::mul:
        return ir::OpCode::mul_plain;
        break;

      case ir::OpCode::sub:
        return ir::OpCode::sub_plain;
        break;

      default:
        return node->get_opcode();
        break;
      }
    }
    else
      return node->get_opcode();
  }
  else
    throw("node with more than 2 operands in deduce_opcode_to_generate");
}

void Translator::write_rotations_steps_getter_def(const std::vector<int32_t> &steps, std::ostream &os_source)
{
  os_source << gen_steps_function_signature << "{\n";
  os_source << "std::vector<" << rotation_step_type_literal << ">"
            << " steps = {";
  for (size_t i = 0; i < steps.size(); i++)
  {
    os_source << steps[i];
    if (i < steps.size() - 1)
      os_source << ",";
    else
      os_source << "};";
  }
  os_source << " return steps; }";
}

void Translator::fix_ir_instruction(const ir::Program::Ptr &node)
{
  if (node->is_operation_node() == false)
    return;

  if (node->get_operands().size() != 2)
    return;

  ir::OpCode opcode = node->get_opcode();

  if (node->get_term_type() == ir::ciphertextType)
  {

    if (
      node->get_operands()[0]->get_term_type() != ir::ciphertextType &&
      node->get_operands()[1]->get_term_type() != ir::ciphertextType)
    {
      throw("ciphertext node with non ciphertext operands");
    }

    if (opcode == ir::OpCode::add || opcode == ir::OpCode::mul)
    {
      if (node->get_operands()[0]->get_term_type() != ir::ciphertextType)
        node->reverse_operands();
    }
    else if (opcode == ir::OpCode::sub)
    {
      ir::Program::Ptr lhs = node->get_operands()[0];
      ir::Program::Ptr rhs = node->get_operands()[1];
      ir::Program::Ptr node_copy = std::make_shared<ir::Term>(*node.get());

      if (lhs->get_term_type() != ir::ciphertextType)
      {
        ir::Program::Ptr negate_node = program->insert_operation_node_in_dataflow(
          ir::OpCode::negate, std::vector<ir::Program::Ptr>({rhs}), "", ir::ciphertextType);
        node->set_opcode(ir::OpCode::add);
        node->clear_operands();
        node->set_operands(std::vector<ir::Program::Ptr>({negate_node, lhs}));
      }
    }
    else if (node->get_opcode() == ir::OpCode::rotate)
    {
      if (
        program->get_targeted_backend() == fhecompiler::Backend::SEAL &&
        program->get_encryption_scheme() != fhecompiler::Scheme::ckks)
      {
        node->set_opcode(ir::OpCode::rotate_rows);
      }
    }
  }
  else if (node->get_term_type() == ir::scalarType)
    throw("only plaintext and ciphertext operation terms are expected");
}

void Translator::fix_ir_instructions_pass()
{
  auto nodes = program->get_dataflow_sorted_nodes(true);
  for (auto &node : nodes)
  {
    /*
      Order of calling the two functions is important
    */
    fix_ir_instruction(node);
  }
}

void Translator::make_scopes_graph(size_t nodes_count_threshold)
{
  // CFG
  auto &nodes = program->get_dataflow_sorted_nodes(true);
  size_t nodes_count(0);
  for (auto &node : nodes)
  {

    if (node->get_term_type() == ir::TermType::rawDataType)
      continue;

    nodes_count++;
    created_in[node] = scope_id;
    scope_nodes_by_id[scope_id].push_back(node);

    if (node->is_operation_node())
    {
      for (auto &operand : node->get_operands())
        accessed_in[operand].insert(scope_id);
    }
    if (nodes_count >= nodes_count_threshold)
    {
      nodes_count = 0;
      scope_id++;
    }
  }
  // building scopes dependency graph
  for (auto &node : nodes)
  {
    for (auto &acc_scope_id : accessed_in[node])
    {
      scopes_graph[acc_scope_id].insert(created_in[node]);
    }
  }
  // sort scopes
  std::unordered_set<size_t> vis;
  for (auto &scope : scopes_graph)
  {
    if (vis.find(scope.first) != vis.end())
      continue;
    sort_scopes_ids(scope.first, vis);
  }
}

void Translator::sort_scopes_ids(size_t scope_id, std::unordered_set<size_t> &visited)
{
  visited.insert(scope_id);
  for (auto &u : scopes_graph[scope_id])
  {
    if (visited.find(u) != visited.end())
      continue;
    sort_scopes_ids(u, visited);
  }
  scopes_ids_sorted.push_back(scope_id);
}

void Translator::generate_scope(size_t _scope_id, std::ofstream &os)
{
  auto &terms_nodes = scope_nodes_by_id[_scope_id];

  for (auto &term_node : terms_nodes)
  {

    if (term_node->is_operation_node())
    {
      for (auto &operand : term_node->get_operands())
      {
        if (operand->get_term_type() == ir::rawDataType)
          continue;

        auto it = parents_copy[operand].find(term_node->get_label());
        if (it != parents_copy[operand].end())
          parents_copy[operand].erase(it);
      }
      for (auto &operand : term_node->get_operands())
      {

        bool is_operand_output =
          program->get_outputs_nodes().find(operand->get_label()) != program->get_outputs_nodes().end();

        if (operand->get_term_type() == ir::TermType::rawDataType)
          continue;

        if (is_operand_output)
          continue;

        if (program->type_of(operand->get_label()) == ir::ConstantTableEntryType::input)
          continue;

        if (parents_copy[operand].size() == 0)
        {
          std::string new_obj_label = current_label[operand];
          ir::TermType operand_t_type = operand->get_term_type();
          if (generated_shareds.find(operand) != generated_shareds.end())
          {
            size_t p_size = free_global_objs[operand_t_type].size();
            if (p_size == 0)
              free_global_objs[operand_t_type].push_back(new_obj_label);
            else
            {
              if (free_global_objs[operand_t_type].back() != new_obj_label)
                free_global_objs[operand_t_type].push_back(new_obj_label);
            }
          }
          else
          {
            size_t p_size = free_local_objs[operand_t_type][created_in[operand]].size();
            if (p_size == 0)
              free_local_objs[operand_t_type][created_in[operand]].push_back(new_obj_label);
            else
            {
              if (free_local_objs[operand_t_type][created_in[operand]].back() != new_obj_label)
                free_local_objs[operand_t_type][created_in[operand]].push_back(new_obj_label);
            }
          }
        }
      }
    }
    translate_term(term_node, os, _scope_id);
  }
}

void Translator::write_as_shared_object(const ir::Program::Ptr &node, std::ofstream &os)
{

  std::string ident = get_identifier(current_label[node]);

  if (node->get_term_type() == ir::TermType::ciphertextType)
  {
    os << shared_ciphers_map_id << "[\"" << ident << "\"] = "
       << "std::move(" << ident << ");\n";
  }
  else
  {
    os << shared_plains_map_id << "[\"" << ident << "\"] = std::move(" << ident << ");\n";
  }
}

void Translator::generate_function_call(std::ofstream &os, const std::string &func_name) const
{
  os << func_name << "(" << inputs_class_identifier[ir::ciphertextType] << ","
     << inputs_class_identifier[ir::plaintextType] << "," << outputs_class_identifier[ir::ciphertextType] << ","
     << outputs_class_identifier[ir::plaintextType] << "," << context_identifier << "," << relin_keys_identifier << ","
     << galois_keys_identifier << "," << public_key_identifier << "," << evaluator_identifier << ","
     << encoder_type_identifier << "," << encryptor_type_literal << ");";
}

void Translator::generate_function_call_class(std::ofstream &os, const std::string &func_name) const
{
  os << func_name << "(" << inputs_class_identifier[ir::ciphertextType] << ","
     << inputs_class_identifier[ir::plaintextType] << "," << outputs_class_identifier[ir::ciphertextType] << ","
     << outputs_class_identifier[ir::plaintextType] << "," << evaluator_identifier << "," << encoder_type_identifier
     << "," << encryptor_type_identifier << ");";
}

void Translator::generate_class_func_signature(std::ofstream &os, const std::string &func_name) const
{

  ArgumentList argument_list;
  os << func_name
     << argument_list(
          {{encrypted_inputs_class_literal, inputs_class_identifier[ir::ciphertextType], AccessType::readAndModify},
           {encoded_inputs_class_literal, inputs_class_identifier[ir::plaintextType], AccessType::readAndModify},
           {encrypted_outputs_class_literal, outputs_class_identifier[ir::ciphertextType], AccessType::readAndModify},
           {encoded_outputs_class_literal, outputs_class_identifier[ir::plaintextType], AccessType::readAndModify},
           {evaluator_type_literal, evaluator_identifier, AccessType::readOnly},
           {bv_encoder_type_literal, encoder_type_identifier, AccessType::readOnly},
           {encryptor_type_literal, encryptor_type_identifier, AccessType::readOnly}})
     << '\n';
}

void Translator::generate_main_func_signature(std::ofstream &os, const std::string &func_name) const
{
  ArgumentList argument_list;
  os << func_name
     << argument_list(
          {{encrypted_inputs_class_literal, inputs_class_identifier[ir::ciphertextType], AccessType::readAndModify},
           {encoded_inputs_class_literal, inputs_class_identifier[ir::plaintextType], AccessType::readAndModify},
           {encrypted_outputs_class_literal, outputs_class_identifier[ir::ciphertextType], AccessType::readAndModify},
           {encoded_outputs_class_literal, outputs_class_identifier[ir::plaintextType], AccessType::readAndModify}})
     << '\n';
}

void Translator::generate_constructor_signature(std::ofstream &os, const std::string &class_name) const
{
  ArgumentList argument_list;
  os << class_name
     << argument_list(
          {{context_type_literal, context_identifier, AccessType::readOnly},
           {relin_keys_type_literal, relin_keys_identifier, AccessType::readOnly},
           {public_key_literal, public_key_identifier, AccessType::readOnly},
           {galois_keys_type_literal, galois_keys_identifier, AccessType::readOnly}})
     << '\n';
}

void Translator::generate_computation_class(std::ofstream &os_header, std::ofstream &os_source)
{
  // Generating the header file
  {
    os_header << "class " << generated_class_name << "{\n";
    os_header << "private:\n";

    // declaring all attributes
    os_header << "std::unordered_map<std::string,seal::Ciphertext> " << shared_ciphers_map_id << ";\n";
    os_header << "std::unordered_map<std::string,seal::Plaintext> " << shared_plains_map_id << ";\n";
    os_header << context_type_literal << " " << context_identifier << ";\n";
    os_header << relin_keys_type_literal << " " << relin_keys_identifier << ";\n";
    os_header << galois_keys_type_literal << " " << galois_keys_identifier << ";\n";
    os_header << public_key_literal << " " << public_key_identifier << ";\n";

    for (auto &_scope_id : scopes_ids_sorted)
    {
      os_header << "void ";
      generate_class_func_signature(os_header, "func_" + std::to_string(_scope_id));
      os_header << ";\n";
    }

    os_header << "public:\n";
    // constructor
    generate_constructor_signature(os_header, generated_class_name);
    os_header << ";\n";
    os_header << "void ";
    generate_main_func_signature(os_header, program->get_program_tag());
    os_header << ";\n";
    os_header << "};\n";
  }

  // generating source file
  {
    define_class_constructor(os_source, generated_class_name);

    for (auto &_scope_id : scopes_ids_sorted)
    {
      os_source << "void " << generated_class_name << "::";
      generate_class_func_signature(os_source, "func_" + std::to_string(_scope_id));
      os_source << "{\n";
      generate_scope(_scope_id, os_source);
      os_source << "}\n";
    }

    define_main_func(os_source, program->get_program_tag());
  }
}

void Translator::define_main_func(std::ofstream &os, const std::string &func_name)
{
  os << "void " << generated_class_name << "::";
  generate_main_func_signature(os, func_name);
  os << "{\n";
  os << evaluator_type_literal << " " << evaluator_identifier << "(" << context_identifier << ");\n";
  os << bv_encoder_type_literal << " " << encoder_type_identifier << "(" << context_identifier << ");\n";
  os << encryptor_type_literal << " " << encryptor_type_identifier << "(" << context_identifier << ","
     << public_key_identifier << ");\n";

  for (auto &scope_id : scopes_ids_sorted)
    write_scope_function_call(scope_id, os);

  for (auto &output_it : program->get_outputs_nodes())
  {
    if (output_it.first != output_it.second->get_label())
    {
      write_output(
        get_output_identifier(output_it.first, output_it.second->get_term_type(), false),
        get_identifier(output_it.second), (output_it.second)->get_term_type(), os);
    }
  }

  os << "}\n";
}

void Translator::define_class_constructor(std::ofstream &os, const std::string &class_name) const
{
  ArgumentList argument_list;
  os << class_name << "::";
  generate_constructor_signature(os, class_name);
  os << " : " << context_identifier << "(" << context_identifier << ")," << relin_keys_identifier << "("
     << relin_keys_identifier << ")," << public_key_identifier << "(" << public_key_identifier << "),"
     << galois_keys_identifier << "(" << galois_keys_identifier << ")"
     << "\n";
  os << "{\n";
  os << "}\n";
}

bool Translator::is_shared_obj(const ir::Program::Ptr &node, size_t term_scope_id)
{
  int32_t accessed_inside = accessed_in[node].find(created_in[node]) != accessed_in[node].end();
  bool is_shared = created_in[node] == term_scope_id && accessed_in[node].size() >= (1 + accessed_inside);
  return is_shared;
}

std::string Translator::get_shared_identifier(const std::string &label, ir::TermType type)
{
  std::string ident = get_identifier(label);

  if (type == ir::TermType::ciphertextType)
  {
    return shared_ciphers_map_id + "[\"" + ident + "\"]";
  }
  else
  {
    return shared_plains_map_id + "[\"" + ident + "\"]";
  }
}

std::string Translator::get_input_identifier(const ir::Program::Ptr &term)
{
  std::string tag;
  if (program->get_entry_form_constants_table(term->get_label()) != std::nullopt)
  {
    ir::ConstantTableEntry table_entry = *(program->get_entry_form_constants_table(term->get_label()));
    ir::ConstantTableEntry::EntryValue entry_value = table_entry.get_entry_value();
    tag = entry_value.get_tag();
  }
  else
    tag = term->get_label();

  return std::string(inputs_class_identifier[term->get_term_type()]) + "[\"" + tag + "\"]";
}

} // namespace translator
