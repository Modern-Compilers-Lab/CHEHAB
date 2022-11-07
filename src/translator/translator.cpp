#include "translator.hpp"
#include "fhecompiler_const.hpp"
#include "program.hpp"
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
      using VectorValue = ir::ConstantTableEntry::VectorValue;
      VectorValue vector_value = std::get<VectorValue>(*(entry_value.value));
      using VectorInt = std::vector<int64_t>;
      using VectorFloat = std::vector<double>;

      type_str = (program->get_encryption_scheme() == fhecompiler::ckks ? scalar_float : scalar_int);

      if (auto _vector_value = std::get_if<VectorInt>(&vector_value))
        encoding_writer.write_vector_encoding(os, tag, *_vector_value, type_str);
      else if (auto vector_literal = std::get_if<VectorFloat>(&vector_value))
        encoding_writer.write_vector_encoding(os, tag, *_vector_value, type_str);
      else
        throw("unsupported data type by schemes\n");
    }

    else if (term_type == ir::scalarType)
    {
      // don't forget to reduce
      using ScalarValue = ir::ConstantTableEntry::ScalarValue;
      ScalarValue scalar_value = std::get<ScalarValue>(*(entry_value.value));

      if (auto value = std::get_if<int64_t>(&scalar_value))
      {
        uint64_t casted_value = static_cast<uint64_t>(*value);
        encoding_writer.write_scalar_encoding(
          os, tag, std::to_string(casted_value), type_str, std::to_string(encryption_parameters->poly_modulus_degree));
      }
      else
      {
        double e_value = std::get<double>(scalar_value);
        encoding_writer.write_scalar_encoding(
          os, tag, std::to_string(e_value), type_str, std::to_string(encryption_parameters->poly_modulus_degree));
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

void Translator::translate_binary_operation(
  const Ptr &term_ptr, std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt,
  std::ofstream &os)
{

  if (!evaluation_writer.is_initialized())
    evaluation_writer.init(os);

  std::string other_args(""); // this depends on the operation
  auto it = get_other_args_by_opcode.find(term_ptr->get_opcode());

  if (it != get_other_args_by_opcode.end())
    other_args = it->second;

  std::string op_identifier = get_identifier(term_ptr);
  auto &operands = *(term_ptr->get_operands());
  std::string lhs_identifier = get_identifier(operands[0]);
  std::string rhs_identifier = get_identifier(operands[1]);
  evaluation_writer.write_binary_operation(
    os, term_ptr->get_opcode(), op_identifier, lhs_identifier, rhs_identifier, term_ptr->get_term_type());
}

void Translator::translate_nary_operation(
  const Ptr &term_ptr, std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt,
  std::ofstream &os)
{
  std::cout << "translation of nary \n";
}

void Translator::translate_unary_operation(
  const Ptr &term_ptr, std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt,
  std::ofstream &os)
{
  std::string op_identifier = get_identifier(term_ptr);
  std::string rhs_identifier = get_identifier((*term_ptr->get_operands())[0]);
  // os << op_type << " " << op_identifier << ops_map[term_ptr->get_opcode()] << rhs_identifier << end_of_command <<
  // '\n';
  if (term_ptr->get_opcode() == ir::OpCode::assign)
  {
    write_assign_operation(os, op_identifier, rhs_identifier, term_ptr->get_term_type());
  }
}

void Translator::translate_term(const Ptr &term, std::ofstream &os)
{

  auto constant_table_entry_opt = program->get_entry_form_constants_table(term->get_label());

  // we need to tranlsate the operation node
  if (term->get_operands() != std::nullopt)
  {
    auto &operands = *(term->get_operands());

    if (operands.size() == 1)
    {
      if (term->get_opcode() == ir::OpCode::encrypt)
      {
        if (!encryption_writer.is_initialized())
        {
          encryption_writer.init(os);
        }
        const std::string &plaintext_id = get_identifier((*term->get_operands())[0]);
        const std::string &destination_cipher = get_identifier(term);
        encryption_writer.write_encryption(os, plaintext_id, destination_cipher);
      }
      else
        translate_unary_operation(term, constant_table_entry_opt, os);
    }
    else if (operands.size() == 2)
    {
      translate_binary_operation(term, constant_table_entry_opt, os);
    }
    else
    {
      std::cout << operands.size() << "\n";
      translate_nary_operation(term, constant_table_entry_opt, os);
    }
  }
  else if (constant_table_entry_opt != std::nullopt)
  {
    ir::ConstantTableEntry &constant_table_entry = *constant_table_entry_opt;
    translate_constant_table_entry(constant_table_entry, term->get_term_type(), os);
  }
}

void Translator::translate(std::ofstream &os)
{

  context_writer.write_context(os);

  generate_function_signature(os);
  os << "{" << '\n';

  const std::vector<Ptr> &nodes_ptr = program->get_dataflow_sorted_nodes();
  /*
  for (auto &node_ptr : nodes_ptr)
  {
    // std::cout << node_ptr->get_parents_labels().size() << "\n";
    translate_term(node_ptr, os);
  }
  for (auto &output_node : program->get_outputs_nodes())
  {
    write_output(get_identifier(output_node.second), (output_node.second)->get_term_type(), os);
  }
  */
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

} // namespace translator
