#include "translator.hpp"
#include "fhecompiler_const.hpp"
#include "program.hpp"
#include "translator_const.hpp"

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
  ir::ConstantTableEntry &table_entry, ir::TermType term_type, std::ofstream &os) const
{
  /*
  basically in this function we need to generate a decalartion or a definition in C++ for a given entry.
  the entry could be representing an input, output, a constant (with raw data) or some temp variable with a tag
  specified by the programmer
  */

  // getting type
  std::string type_str;
  if (term_type == ir::TermType::scalarType)
  {
    fhecompiler::Scheme scheme = program->get_encryption_scheme();
    if (scheme == fhecompiler::bfv || scheme == fhecompiler::bgv)
      type_str = scalar_int;
    else if (scheme == fhecompiler::ckks)
      type_str = scalar_float;
    else
      throw("scheme not supported\n");
  }
  else
    type_str = types_map[term_type];

  ir::ConstantTableEntry::EntryValue entry_value = table_entry.get_entry_value();
  ir::ConstantTableEntryType entry_type = table_entry.get_entry_type();
  if (entry_type == ir::ConstantTableEntryType::input)
  {
    os << type_str << " " << entry_value.get_tag() << " = " << inputs_map_by_type[term_type] << open_bracket
       << entry_value.get_tag() << close_bracket << end_of_command << '\n';
  }
  else if (entry_type == ir::ConstantTableEntryType::constant)
  {
    if (term_type == ir::ciphertextType)
    {
      // encryption
    }
    else if (term_type == ir::plaintextType)
    {
      // encoding
    }
    else if (term_type == ir::scalarType)
    {
      // define and int
    }
    else
      throw("type not supported ");
  }
}

void Translator::translate_binary_operation(
  const Ptr &term_ptr, std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt,
  std::ofstream &os) const
{
  std::string op_identifier = get_identifier(term_ptr);
  const std::vector<Ptr> &operands = *(term_ptr->get_operands());
  std::string lhs_identifier = get_identifier(operands[0]);
  std::string rhs_identifier = get_identifier(operands[1]);
  std::string op_type = types_map[term_ptr->get_term_type()];
  os << op_type << " " << op_identifier << " = " << ops_map[term_ptr->get_opcode()] << open_parantesis << lhs_identifier
     << "," << rhs_identifier << close_parantesis << end_of_command << '\n';
}
void Translator::translate_nary_operation(
  const Ptr &term_ptr, std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt,
  std::ofstream &os) const
{}
void Translator::translate_unary_operation(
  const Ptr &term_ptr, std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt,
  std::ofstream &os) const
{
  std::string op_identifier = get_identifier(term_ptr);
  std::string rhs_identifier = get_identifier((*term_ptr->get_operands())[0]);
  std::string op_type = types_map[term_ptr->get_term_type()];
  os << op_type << " " << op_identifier << ops_map[term_ptr->get_opcode()] << rhs_identifier << end_of_command << '\n';
}

void Translator::translate_term(const Ptr &term, std::ofstream &os) const
{

  auto constant_table_entry_opt = program->get_entry_form_constants_table(term->get_label());

  // we need to tranlsate the operation node
  if (term->get_operands() != std::nullopt)
  {
    const std::vector<Ptr> &operands = *(term->get_operands());

    if (operands.size() == 1)
    {
      translate_unary_operation(term, constant_table_entry_opt, os);
    }
    else if (operands.size() == 2)
    {
      translate_binary_operation(term, constant_table_entry_opt, os);
    }
    else
    {
      translate_nary_operation(term, constant_table_entry_opt, os);
    }
  }
  else if (constant_table_entry_opt != std::nullopt)
  {
    ir::ConstantTableEntry &constant_table_entry = *constant_table_entry_opt;
    translate_constant_table_entry(constant_table_entry, term->get_term_type(), os);
  }
}

void Translator::translate(std::ofstream &os) const
{
  generate_function_signature(os);
  os << start_block << '\n';
  const std::vector<Ptr> nodes_ptr = program->get_dataflow_sorted_nodes();
  for (auto &node_ptr : nodes_ptr)
  {
    translate_term(node_ptr, os);
  }
  os << end_block << '\n';
}

void Translator::generate_function_signature(std::ofstream &os) const
{
  os << vector_literal << "<" << types_map[ir::ciphertextType] << ">"
     << " " << program->get_program_tag() << open_parantesis << inputs_map_by_type[ir::ciphertextType] << ","
     << inputs_map_by_type[ir::plaintextType] << close_parantesis << '\n';
}

} // namespace translator
