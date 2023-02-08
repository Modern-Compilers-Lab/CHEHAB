#include "translator.hpp"
#include "cse_pass.hpp"
#include "fhecompiler_const.hpp"
#include "ir_const.hpp"
#include "program.hpp"
#include "rotationkeys_select_pass.hpp"
#include <fstream>

namespace translator
{

std::string Translator::get_identifier(const Ptr &term_ptr) const
{
  if (program->get_entry_form_constants_table(term_ptr->get_label()) != std::nullopt)
  {
    ir::ConstantTableEntry table_entry = *(program->get_entry_form_constants_table(term_ptr->get_label()));
    ir::ConstantTableEntry::EntryValue entry_value = table_entry.get_entry_value();
    return entry_value.get_tag();
  }
  else
    return term_ptr->get_label();
}

/*
void Translator::compact_assignement(const ir::Term::Ptr &node_ptr)
{
  if (!node_ptr->is_operation_node())
    return;

  if (node_ptr->get_opcode() != ir::OpCode::assign)
    return;

  auto operand = node_ptr->get_operands()[0];

  if (operand->is_operation_node() && operand->get_opcode() == ir::OpCode::assign)
  {
    node_ptr->delete_operand_at_index(0);
    node_ptr->add_operand(operand->get_operands()[0]);
    std::optional<std::string> new_tag =
      program->get_tag_value_in_constants_table_entry_if_exists(operand->get_label());

    if (new_tag != std::nullopt)
    {
      program->update_tag_value_in_constants_table_entry(node_ptr->get_label(), *new_tag);
    }
  }
}
*/

void Translator::convert_to_square(const ir::Term::Ptr &node_ptr)
{
  /* this function converts mul operation to square operation when it is possible */
  if (!node_ptr->is_operation_node())
    return;

  ir::OpCode opcode = node_ptr->get_opcode();

  if (opcode == ir::OpCode::mul)
  {
    auto &operands = node_ptr->get_operands();
    if (operands.size() != 2)
    {
      throw("got an unexpcted number of operands, expected number is 2 in covert_to_square");
    }

    auto &lhs_ptr = operands[0];
    auto &rhs_ptr = operands[1];
    if (lhs_ptr->get_label() == rhs_ptr->get_label())
    {
      // convert to square
      node_ptr->set_opcode(ir::OpCode::square);
      node_ptr->clear_operands();
      node_ptr->add_operand(lhs_ptr);
      lhs_ptr->delete_parent(node_ptr->get_label()); /* remove a parent once, it doesn't matter whether to remove from
                                                        rhs or lhs since they point to the same object */
    }
  }
}

void Translator::convert_to_inplace(const ir::Term::Ptr &node_ptr)
{
  if (!node_ptr->is_operation_node())
    return;

  auto &operands = node_ptr->get_operands();

  if (operands.size() == 0)
    throw("unexpected size of operands, 0 operands");

  if (program->type_of(node_ptr->get_label()) == ir::ConstantTableEntryType::output)
    return;

  std::unordered_set<ir::OpCode> instructions_to_be_converted = {
    ir::OpCode::relinearize, ir::OpCode::modswitch}; // these instructions needs to be converted since those  are
                                                     // usually applied directly on a ciphertext

  bool conversion_condition =
    (instructions_to_be_converted.find(node_ptr->get_opcode()) != instructions_to_be_converted.end());

  if (operands.size() == 1)
  {
    auto &operand_ptr = operands[0];
    ir::OpCode opcode = node_ptr->get_opcode();

    if (opcode == ir::OpCode::assign || opcode == ir::OpCode::encrypt)
      return;
    else
    {
      operand_ptr->delete_parent(node_ptr->get_label());

      bool is_an_output_operand = (program->type_of(operand_ptr->get_label()) != ir::ConstantTableEntryType::output);

      if (program->type_of(operand_ptr->get_label()) == ir::ConstantTableEntryType::input && !node_ptr->is_inplace())
        return;

      if (is_an_output_operand && !conversion_condition)
        return;

      // an additional condition to convert to inplace implicitly

      bool dependency_condition = operand_ptr->get_parents_labels().size() == 0;

      conversion_condition = conversion_condition || dependency_condition;

      if (conversion_condition)
      {
        program->insert_new_entry_from_existing_with_delete(operand_ptr->get_label(), node_ptr->get_label());
        node_ptr->set_label(operand_ptr->get_label());
      }
    }
  }
  else if (operands.size() == 2)
  {
    auto &lhs_ptr = operands[0];
    auto &rhs_ptr = operands[1];
    ir::OpCode opcode = node_ptr->get_opcode();

    rhs_ptr->delete_parent(node_ptr->get_label());
    lhs_ptr->delete_parent(node_ptr->get_label());

    // an additional condition to convert to inplace implicitly

    bool commutative = (node_ptr->get_opcode() == ir::OpCode::add) || (node_ptr->get_opcode() == ir::OpCode::mul);

    if (program->type_of(lhs_ptr->get_label()) == ir::ConstantTableEntryType::input && !commutative)
      return;

    bool is_lhs_an_output = program->type_of(operands[0]->get_label()) == ir::ConstantTableEntryType::output;
    bool is_rhs_an_output = program->type_of(operands[1]->get_label()) == ir::ConstantTableEntryType::output;

    bool is_rhs_an_input = program->type_of(operands[1]->get_label()) == ir::ConstantTableEntryType::input;

    if (
      commutative && operands[0]->get_term_type() == ir::ciphertextType &&
      operands[1]->get_term_type() == ir::ciphertextType &&
      operands[0]->get_parents_labels().size() > operands[1]->get_parents_labels().size() && !is_rhs_an_output &&
      !is_rhs_an_input)
    {
      node_ptr->reverse_operands();
    }

    if (program->type_of(operands[0]->get_label()) == ir::ConstantTableEntryType::constant)
      return;

    if (program->type_of(operands[0]->get_label()) == ir::ConstantTableEntryType::input)
      return;

    if (
      program->type_of(operands[0]->get_label()) == ir::ConstantTableEntryType::output &&
      !conversion_condition) // in this case no need to check dependency condition since the lhs is an output, checking
                             // !conversion_condition is enough
      return;

    bool dependency_condition = lhs_ptr->get_parents_labels().size() == 0;

    conversion_condition = conversion_condition || dependency_condition;

    if (conversion_condition)
    {
      program->insert_new_entry_from_existing_with_delete(lhs_ptr->get_label(), node_ptr->get_label());
      node_ptr->set_label(lhs_ptr->get_label());
    }
  }
  else
    throw("unexpected size of operands, more than 2");
}

void Translator::translate_constant_table_entry(
  ir::ConstantTableEntry &table_entry, ir::TermType term_type, std::ofstream &os)
{
  /*
    basically in this function we need to generate a decalartion or a definition in C++ for a given entry.
    the entry could be representing an input, output, a constant (with raw data) or some temp variable with a tag
    specified by the programmer
  */

  // Retrieving needed information
  ir::ConstantTableEntry::EntryValue entry_value = table_entry.get_entry_value();
  ir::ConstantTableEntryType entry_type = table_entry.get_entry_type();
  std::string tag = entry_value.get_tag();
  // getting type
  std::string type_str;

  if (term_type == ir::TermType::scalarType)
  {
    fhecompiler::Scheme scheme = program->get_encryption_scheme();
    if (scheme == fhecompiler::bfv || scheme == fhecompiler::bgv)
      type_str = scalar_int;
    else if (scheme == fhecompiler::ckks)
    {
      /*
      using ScalarValue = ir::ConstantTableEntry::ScalarValue;
      ScalarValue scalar_value = std::get<ScalarValue>(*(entry_value.value));
      if (auto scalar_constant = std::get_if<int64_t>(&scalar_value))
      {
        type_str = scalar_int;
      }
      */
      type_str = scalar_float;
    }
    else
      throw("scheme not supported\n");
  }
  else
    type_str = types_map[term_type];

  if (entry_type == ir::ConstantTableEntryType::input)
  {
    write_input(tag, term_type, os);
  }
  else if (entry_type == ir::ConstantTableEntryType::constant)
  {
    if (!encoding_writer.is_initialized())
    {
      encoding_writer.init(os);
    }
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
          os, tag, *_vector_value, type_str,
          program->get_encryption_scheme() == fhecompiler::Scheme::ckks ? program->get_scale() : 0.0);
      else if (auto vector_literal = std::get_if<VectorFloat>(&vector_value))
        encoding_writer.write_vector_encoding(os, tag, *_vector_value, type_str, program->get_scale());
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
          os, tag, std::to_string(casted_value), type_str, std::to_string(program->get_number_of_slots()),
          program->get_encryption_scheme() == fhecompiler::Scheme::ckks ? program->get_scale() : 0.0);
      }
      else
      {
        double e_value = std::get<double>(scalar_value);
        encoding_writer.write_scalar_encoding(
          os, tag, std::to_string(e_value), type_str, std::to_string(program->get_number_of_slots()),
          program->get_scale());
      }
      /*
      if (type_str == scalar_int)
      {
        // bfv/bgv
        // we call reduce
        uint64_t value = static_cast<uint64_t>(std::get<int64_t>(scalar_value)); // later we change that
        encoding_writer.write_scalar_encoding(
          os, tag, std::to_string(value), type_str, std::to_string(program->get_dimension()));
      }
      else if (type_str == scalar_float)
      {
        // !
        // we call reduce
        double value = static_cast<double>(std::get<double>(scalar_value)); // later we change that
        encoding_writer.write_scalar_encoding(
          os, tag, std::to_string(value), type_str, std::to_string(program->get_dimension()));
      }
      */
    }
    else
      throw("type not supported ");
  }
}

void Translator::translate_binary_operation(const Ptr &term_ptr, std::ofstream &os)
{
  std::string other_args(""); // this depends on the operation
  auto it = get_other_args_by_opcode.find(term_ptr->get_opcode());

  if (it != get_other_args_by_opcode.end())
    other_args = it->second;

  std::string op_identifier = get_identifier(term_ptr);
  auto &operands = term_ptr->get_operands();
  std::string lhs_identifier = get_identifier(operands[0]);
  std::string rhs_identifier = get_identifier(operands[1]);

  evaluation_writer.write_binary_operation(
    os, deduce_opcode_to_generate(term_ptr), op_identifier, lhs_identifier, rhs_identifier, term_ptr->get_term_type());
}

void Translator::translate_nary_operation(const Ptr &term_ptr, std::ofstream &os)
{
  std::cout << "translation of nary \n";
}

void Translator::translate_unary_operation(const Ptr &term_ptr, std::ofstream &os)
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
      os, term_ptr->get_opcode(), op_identifier, rhs_identifier, term_ptr->get_term_type());
  }
}

void Translator::translate_term(const Ptr &term, std::ofstream &os)
{

  auto constant_table_entry_opt = program->get_entry_form_constants_table(term->get_label());

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
        if (!encryption_writer.is_initialized())
        {
          encryption_writer.init(os);
        }
        const std::string &plaintext_id = get_identifier(term->get_operands()[0]);
        const std::string &destination_cipher = get_identifier(term);
        encryption_writer.write_encryption(os, plaintext_id, destination_cipher);
      }
      else
        translate_unary_operation(term, os);
    }
    else if (operands.size() == 2)
    {
      translate_binary_operation(term, os);
    }
    else
    {
      translate_nary_operation(term, os);
    }
  }
  else if (constant_table_entry_opt != std::nullopt)
  {
    ir::ConstantTableEntry &constant_table_entry = *constant_table_entry_opt;
    translate_constant_table_entry(constant_table_entry, term->get_term_type(), os);
  }
}

void Translator::translate_program(std::ofstream &os)
{

  os << headers_include;

  context_writer.write_context(os);

  os << "\n";

  write_rotations_steps_getter(program->get_rotations_steps(), os);

  generate_function_signature(os);
  os << "{" << '\n';

  convert_to_inplace_pass();

  {
    const std::vector<Ptr> &nodes_ptr = program->get_dataflow_sorted_nodes(false);

    // after doing all passes, now we do the last pass to translate and generate the code
    for (auto &node_ptr : nodes_ptr)
    {
      translate_term(node_ptr, os);
    }
  }

  for (auto &output_node : program->get_outputs_nodes())
  {

    if (output_node.second->is_operation_node() == false) /* Output node must be at least an assignement node */
      continue;

    write_output(get_identifier(output_node.second), (output_node.second)->get_term_type(), os);
  }

  os << "}" << '\n';
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

void Translator::write_input(const std::string &input_identifier, ir::TermType type, std::ostream &os)
{
  // retrieve an input from object
  os << types_map[type] << " " << input_identifier << " = " << inputs_class_identifier[type] << "["
     << stringfy_string(input_identifier) << "];" << '\n';
}

void Translator::write_output(const std::string &output_identifier, ir::TermType type, std::ostream &os)
{
  // insert output in object
  os << outputs_class_identifier[type] << "." << insert_object_instruction << "({" << stringfy_string(output_identifier)
     << "," << output_identifier << "})"
     << ";" << '\n';
}

void Translator::write_assign_operation(
  std::ofstream &os, const std::string &lhs_id, const std::string &rhs_id, ir::TermType type)
{
  os << types_map[type] << " " << lhs_id << " = " << rhs_id << ";" << '\n';
}

void Translator::convert_to_inplace_pass()
{
  auto &sorted_nodes = program->get_dataflow_sorted_nodes(true);
  for (auto &node_ptr : sorted_nodes)
  {
    convert_to_inplace(node_ptr);
  }
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

void Translator::write_rotations_steps_getter(const std::vector<int32_t> &steps, std::ostream &os)
{
  os << gen_steps_function_signature << "{\n";
  os << "std::vector<" << rotation_step_type_literal << ">"
     << " steps = {";
  for (size_t i = 0; i < steps.size(); i++)
  {
    os << steps[i];
    if (i < steps.size() - 1)
      os << ",";
    else
      os << "};";
  }
  os << " return steps; }";
}

} // namespace translator
