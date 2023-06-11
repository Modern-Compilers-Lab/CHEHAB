#include "fheco/code_gen/constants.hpp"
#include "fheco/code_gen/gen_func.hpp"
#include "fheco/ir/func.hpp"
#include <algorithm>
#include <iterator>

using namespace std;

namespace fheco::code_gen
{
void gen_func(
  const shared_ptr<ir::Func> &func, const unordered_set<int> &rotataion_steps, ostream &header_os,
  string_view header_name, ostream &source_os)
{
  header_os << header_includes;
  header_os << '\n';
  gen_func_decl(func->name(), header_os);
  header_os << '\n';
  gen_rotation_steps_getter_decl(func->name(), header_os);

  source_os << source_includes;
  source_os << "#include \"" << header_name << "\"\n";
  source_os << '\n';
  source_os << source_usings;
  source_os << '\n';
  gen_func_def_signature(func->name(), source_os);
  source_os << "\n{\n";

  TermsObjectsInfo terms_objects_info;
  gen_input_terms(func->data_flow().inputs_info(), source_os, terms_objects_info);
  gen_const_terms(
    func->data_flow().constants_info(), func->clear_data_evaluator().signedness(), source_os, terms_objects_info);
  gen_op_terms(func, source_os, terms_objects_info);
  gen_output_terms(func->data_flow().outputs_info(), source_os, terms_objects_info);

  source_os << "}\n";
  source_os << '\n';
  gen_rotation_steps_getter_def(func->name(), rotataion_steps, source_os);
}

void gen_func_decl(const string &func_name, ostream &os)
{
  os << "void " << func_name << "(";
  os << "const " << header_encrypted_io_type << " &" << encrypted_inputs_container_id << ",\n";
  os << "const " << header_encoded_io_type << " &" << encoded_inputs_container_id << ",\n";
  os << header_encrypted_io_type << " &" << encrypted_outputs_container_id << ",\n";
  os << header_encoded_io_type << " &" << encoded_outputs_container_id << ",\n";
  os << "const " << seal_namespace << "::" << encoder_type << " &" << encoder_id << ",\n";
  os << "const " << seal_namespace << "::" << encryptor_type << " &" << encryptor_id << ",\n";
  os << "const " << seal_namespace << "::" << evaluator_type << " &" << evaluator_id << ",\n";
  os << "const " << seal_namespace << "::" << relin_keys_type << " &" << relin_keys_id << ",\n";
  os << "const " << seal_namespace << "::" << galois_keys_type << " &" << galois_keys_id << ");\n";
}

void gen_rotation_steps_getter_decl(const string &func_name, ostream &os)
{
  os << "std::vector<int> " << rotation_steps_getter_id + "_" + func_name << "();\n";
}

void gen_func_def_signature(const string &func_name, ostream &os)
{
  os << "void " << func_name << "(";
  os << "const " << source_encrypted_io_type << " &" << encrypted_inputs_container_id << ",\n";
  os << "const " << source_encoded_io_type << " &" << encoded_inputs_container_id << ",\n";
  os << source_encrypted_io_type << " &" << encrypted_outputs_container_id << ",\n";
  os << source_encoded_io_type << " &" << encoded_outputs_container_id << ",\n";
  os << "const " << encoder_type << " &" << encoder_id << ",\n";
  os << "const " << encryptor_type << " &" << encryptor_id << ",\n";
  os << "const " << evaluator_type << " &" << evaluator_id << ",\n";
  os << "const " << relin_keys_type << " &" << relin_keys_id << ",\n";
  os << "const " << galois_keys_type << " &" << galois_keys_id << ")";
}

void gen_input_terms(const ir::IOTermsInfo &input_terms_info, ostream &os, TermsObjectsInfo &terms_objects_info)
{
  for (const auto &input_info : input_terms_info)
  {
    auto term = input_info.first;
    auto object_id = term->id();
    terms_objects_info.emplace(term->id(), ObjectInfo{object_id, term->parents().size()});

    if (term->type() == ir::Term::Type::cipher)
    {
      os << cipher_type << " ";
      gen_cipher_var_id(object_id, os);
      os << " = " << encrypted_inputs_container_id << ".at(\"" << input_info.second.label_ << "\")";
    }
    else
    {
      os << plain_type << " ";
      gen_plain_var_id(object_id, os);
      os << " = " << encoded_inputs_container_id << ".at(\"" << input_info.second.label_ << "\")";
    }
    os << ";\n";
  }
}

void gen_cipher_var_id(size_t term_id, ostream &os)
{
  os << "c" << term_id;
}

void gen_plain_var_id(size_t term_id, ostream &os)
{
  os << "p" << term_id;
}

void gen_const_terms(
  const ir::ConstTermsValues &const_terms_info, bool signedness, ostream &os, TermsObjectsInfo &terms_objects_info)
{
  os << "size_t " << slot_count_id << " = " << encoder_id << ".slot_count();\n";
  for (const auto &const_info : const_terms_info)
  {
    auto term = const_info.first;
    auto object_id = term->id();
    terms_objects_info.emplace(term->id(), ObjectInfo{object_id, term->parents().size()});

    os << plain_type << " ";
    gen_plain_var_id(object_id, os);
    os << ";\n";
    os << encoder_id << ".encode(vector<";
    if (signedness)
      os << signed_slot_type;
    else
      os << unsigned_slot_type;
    os << ">";
    if (const_info.second.is_scalar_)
      os << "(" << slot_count_id << ", " << const_info.second.val_[0] << "), ";
    else
    {
      os << "{";
      gen_sequence(const_info.second.val_.cbegin(), const_info.second.val_.cend(), line_threshold, os);
      os << "},\n";
    }
    gen_plain_var_id(object_id, os);
    os << ");\n";
  }
}

void gen_op_terms(const shared_ptr<ir::Func> &func, ostream &os, TermsObjectsInfo &terms_objects_info)
{
  for (auto term : func->get_top_sorted_terms())
  {
    if (!term->is_operation())
      continue;

    size_t term_object_id = term->id();
    vector<size_t> operands_objects_id(term->operands().size());
    for (size_t i = 0; i < operands_objects_id.size(); ++i)
    {
      auto operand = term->operands()[i];
      if (func->data_flow().is_output(operand))
        continue;

      auto operand_object_info_it = terms_objects_info.find(operand->id());
      // operand with multiplicity > 1
      if (operand_object_info_it == terms_objects_info.end())
        continue;

      auto &operand_object_info = operand_object_info_it->second;
      operands_objects_id[i] = operand_object_info.id_;
      --operand_object_info.dep_count_;
      if (term_object_id == term->id() && operand_object_info.dep_count_ == 0)
      {
        term_object_id = operands_objects_id[i];
        terms_objects_info.erase(operand_object_info_it);
      }
    }

    if (term_object_id == term->id())
    {
      for (auto it = terms_objects_info.begin(); it != terms_objects_info.end();)
      {
        if (it->second.dep_count_ == 0)
        {
          term_object_id = it->second.id_;
          terms_objects_info.erase(it);
          break;
        }
        ++it;
      }
    }
    terms_objects_info.emplace(term->id(), ObjectInfo{term_object_id, term->parents().size()});

    if (term_object_id == term->id())
    {
      os << cipher_type << " ";
      gen_cipher_var_id(term_object_id, os);
      os << ";\n";
    }

    vector<ir::Term::Type> operands_types;
    operands_types.reserve(term->operands().size());
    transform(
      term->operands().cbegin(), term->operands().cend(), std::back_inserter(operands_types),
      [](const ir::Term *operand) { return operand->type(); });

    if (term->op_code() == ir::OpCode::encrypt)
    {
      os << encryptor_id << ".encrypt(";
      gen_plain_var_id(term->operands()[0]->id(), os);
    }
    else
    {
      os << evaluator_id << "." << operation_mapping.at(OpType{term->op_code().type(), move(operands_types)}) << "(";
      for (size_t i = 0; i < operands_objects_id.size(); ++i)
      {
        auto operand = term->operands()[i];

        if (operand->type() == ir::Term::Type::cipher)
          gen_cipher_var_id(operands_objects_id[i], os);
        else
          gen_plain_var_id(operands_objects_id[i], os);

        if (i + 1 == operands_objects_id.size())
          break;

        os << ", ";
      }
      if (term->op_code().generators().size())
      {
        os << ", ";
        for (auto it = term->op_code().generators().cbegin();;)
        {
          auto generator = *it;
          os << generator;
          ++it;
          if (it == term->op_code().generators().cend())
            break;

          os << ", ";
        }
      }
      if (term->op_code().type() == ir::OpCode::Type::relin)
        os << ", " << relin_keys_id;

      if (term->op_code().type() == ir::OpCode::Type::rotate)
        os << ", " << galois_keys_id;
    }

    os << ", ";
    gen_cipher_var_id(term_object_id, os);

    os << ");\n";
  }
}

void gen_output_terms(const ir::IOTermsInfo &output_terms_info, ostream &os, const TermsObjectsInfo &terms_objects_info)
{
  for (const auto &output_info : output_terms_info)
  {
    auto term = output_info.first;
    size_t term_object;
    term_object = terms_objects_info.at(term->id()).id_;

    if (term->type() == ir::Term::Type::cipher)
    {
      os << encrypted_outputs_container_id << ".emplace(\"" << output_info.second.label_ << "\", ";
      os << "move(";
      gen_cipher_var_id(term_object, os);
    }
    else
    {
      os << encoded_outputs_container_id << ".emplace(\"" << output_info.second.label_ << "\", ";
      os << "move(";
      gen_plain_var_id(term_object, os);
    }
    os << "));\n";
  }
}

void gen_rotation_steps_getter_def(const string &func_name, const unordered_set<int> &steps, ostream &os)
{
  os << "vector<int> " << rotation_steps_getter_id + "_" + func_name << "(){\n";
  os << "return vector<int>{";
  gen_sequence(steps.cbegin(), steps.cend(), line_threshold, os);
  os << "};\n";
  os << "}\n";
}
} // namespace fheco::code_gen
