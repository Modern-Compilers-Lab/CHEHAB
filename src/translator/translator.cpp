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
  ir::ConstantTableEntry &table_entry, ir::TermType term_type, std::ofstream &os, Encoder &encoder) const
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
      using ScalarValue = ir::ConstantTableEntry::ScalarValue;
      ScalarValue scalar_value = std::get<ScalarValue>(*(entry_value.value));
      if (auto scalar_constant = std::get_if<int64_t>(&scalar_value))
      {
        type_str = scalar_int;
      }
    }
    else
      throw("scheme not supported\n");
  }
  else
    type_str = types_map[term_type];

  if (entry_type == ir::ConstantTableEntryType::input)
  {
    MAP_ACCESSOR map_accessor;
    os << type_str << " " << tag << " = "
       << map_accessor(inputs_map_identifier_by_type[term_type], stringfy_string(tag)) << end_of_command << '\n';
  }
  else if (entry_type == ir::ConstantTableEntryType::constant)
  {
    if (encoder.is_defined == false)
    {
      encoder.is_defined = true;
      os << encoder << end_of_command << '\n';
    }

    if (term_type == ir::ciphertextType)
    {
      // encryption
    }
    else if (term_type == ir::plaintextType)
    {
      // encoding
      using VectorValue = ir::ConstantTableEntry::VectorValue;
      VectorValue vector_value = std::get<VectorValue>(*(entry_value.value));
      using VectorInt = std::vector<int64_t>;
      using VectorFloat = std::vector<double>;
      if (auto vector_literal = std::get_if<VectorInt>(&vector_value))
        encoder.write_encode(*vector_literal, tag, os, scalar_int);
      else if (auto vector_literal = std::get_if<VectorFloat>(&vector_value))
        encoder.write_encode(*vector_literal, tag, os, scalar_float);
      else
        throw("unsupported data type by schemes\n");
    }
    else if (term_type == ir::scalarType)
    {
      // don't forget to reduce
      using ScalarValue = ir::ConstantTableEntry::ScalarValue;
      ScalarValue scalar_value = std::get<ScalarValue>(*(entry_value.value));
      if (type_str == scalar_int)
      {
        // bfv/bgv
        // we call reduce
        uint64_t value = static_cast<uint64_t>(std::get<int64_t>(scalar_value)); // later we change that
        encoder.write_encode(std::vector<uint64_t>{value}, tag, os, scalar_int);
      }
      else if (type_str == scalar_float)
      {
        // ckks
        // we call reduce
        double value = static_cast<double>(std::get<double>(scalar_value)); // later we change that
        encoder.write_encode(std::vector<double>{value}, tag, os, scalar_int);
      }
    }
    else
      throw("type not supported ");
  }
}

void Translator::translate_binary_operation(
  const Ptr &term_ptr, std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt,
  std::ofstream &os, const Evaluator &evaluator) const
{
  std::string op_identifier = get_identifier(term_ptr);
  const std::vector<Ptr> &operands = *(term_ptr->get_operands());
  std::string lhs_identifier = get_identifier(operands[0]);
  std::string rhs_identifier = get_identifier(operands[1]);
  evaluator.write_binary_operation(
    term_ptr->get_term_type(), term_ptr->get_opcode(), op_identifier, lhs_identifier, rhs_identifier, os);
}

void Translator::translate_nary_operation(
  const Ptr &term_ptr, std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt,
  std::ofstream &os, const Evaluator &evaluator) const
{}

void Translator::translate_unary_operation(
  const Ptr &term_ptr, std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt,
  std::ofstream &os, const Evaluator &evaluator) const
{
  std::string op_identifier = get_identifier(term_ptr);
  std::string rhs_identifier = get_identifier((*term_ptr->get_operands())[0]);
  std::string op_type = types_map[term_ptr->get_term_type()];
  os << op_type << " " << op_identifier << ops_map[term_ptr->get_opcode()] << rhs_identifier << end_of_command << '\n';
}

void Translator::translate_term(const Ptr &term, std::ofstream &os, const Evaluator &evaluator, Encoder &encoder) const
{

  auto constant_table_entry_opt = program->get_entry_form_constants_table(term->get_label());

  // we need to tranlsate the operation node
  if (term->get_operands() != std::nullopt)
  {
    const std::vector<Ptr> &operands = *(term->get_operands());

    if (operands.size() == 1)
    {
      translate_unary_operation(term, constant_table_entry_opt, os, evaluator);
    }
    else if (operands.size() == 2)
    {
      translate_binary_operation(term, constant_table_entry_opt, os, evaluator);
    }
    else
    {
      translate_nary_operation(term, constant_table_entry_opt, os, evaluator);
    }
  }
  else if (constant_table_entry_opt != std::nullopt)
  {
    ir::ConstantTableEntry &constant_table_entry = *constant_table_entry_opt;
    translate_constant_table_entry(constant_table_entry, term->get_term_type(), os, encoder);
  }
}

void Translator::translate(std::ofstream &os) const
{

  generate_function_signature(os);
  os << start_block << '\n';

  Evaluator evaluator(context_identifier, evaluator_identifier);

  Encoder encoder(context_identifier); // for now we assume that we have only one context

  os << evaluator << end_of_command << '\n';
  const std::vector<Ptr> nodes_ptr = program->get_dataflow_sorted_nodes();
  for (auto &node_ptr : nodes_ptr)
  {
    translate_term(node_ptr, os, evaluator, encoder);
  }
  os << end_block << '\n';
}

void Translator::generate_function_signature(std::ofstream &os) const
{
  CONTAINER_OF container_of;
  ARGUMENTS_LIST argument_list;
  os << "void " << program->get_program_tag()
     << argument_list(
          {{inputs_map_by_type[ir::ciphertextType], inputs_map_identifier_by_type[ir::ciphertextType],
            AccessType::readOnly},
           {inputs_map_by_type[ir::plaintextType], inputs_map_identifier_by_type[ir::plaintextType],
            AccessType::readOnly},
           {outputs_map_by_type[ir::plaintextType], outputs_map_identifier_by_type[ir::plaintextType],
            AccessType::readAndModify},
           {outputs_map_by_type[ir::ciphertextType], outputs_map_identifier_by_type[ir::ciphertextType],
            AccessType::readAndModify},
           {context_type_literal, context_identifier, AccessType::readOnly},
           {relin_keys_type_literal, relin_keys_identifier, AccessType::readOnly},
           {galois_keys_type_literal, galois_keys_identifier, AccessType::readOnly}})
     << '\n';
}

} // namespace translator
