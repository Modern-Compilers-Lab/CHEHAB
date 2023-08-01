#include "fheco/code_gen/constants.hpp"
#include "fheco/code_gen/gen_func.hpp"
#include "fheco/ir/common.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/passes/prepare_code_gen.hpp"
#include <algorithm>
#include <iterator>

using namespace std;

namespace fheco::code_gen
{
void gen_func(
  const shared_ptr<ir::Func> &func, const unordered_set<int> &rotataion_steps, ostream &header_os,
  string_view header_name, ostream &source_os)
{
  passes::prepare_code_gen(func);
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

  TermsCtxtObjectsInfo terms_ctxt_objects_info;
  gen_input_terms(func->data_flow().inputs_info(), source_os, terms_ctxt_objects_info);
  gen_const_terms(func->data_flow().constants_info(), func->clear_data_evaluator().signedness(), source_os);
  gen_op_terms(func, source_os, terms_ctxt_objects_info);
  gen_output_terms(func->data_flow().outputs_info(), source_os, terms_ctxt_objects_info);

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

void gen_input_terms(
  const ir::InputTermsInfo &input_terms_info, ostream &os, TermsCtxtObjectsInfo &terms_ctxt_objects_info)
{
  for (const auto &input_info : input_terms_info)
  {
    auto term = input_info.first;
    auto object_id = term->id();
    if (term->type() == ir::Term::Type::cipher)
    {
      terms_ctxt_objects_info.emplace(term->id(), CtxtObjectInfo{object_id, term->parents().size()});
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

void gen_const_terms(const ir::ConstTermsValues &const_terms_info, bool signedness, ostream &os)
{
  if (const_terms_info.size())
    os << "size_t " << slot_count_id << " = " << encoder_id << ".slot_count();\n";
  for (const auto &const_info : const_terms_info)
  {
    auto term = const_info.first;
    auto object_id = term->id();
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

void gen_op_terms(const shared_ptr<ir::Func> &func, ostream &os, TermsCtxtObjectsInfo &terms_ctxt_objects_info)
{
  for (auto term : func->get_top_sorted_terms())
  {
    if (!term->is_operation())
      continue;

    auto term_object_id = term->id();
    vector<size_t> operands_ctxt_objects_ids(term->operands().size());
    unordered_map<size_t, size_t> operands_multip;
    for (size_t i = 0; i < operands_ctxt_objects_ids.size(); ++i)
    {
      auto operand = term->operands()[i];
      if (operand->type() != ir::Term::Type::cipher)
        continue;

      auto multip = ++operands_multip[operand->id()];
      auto operand_object_info_it = terms_ctxt_objects_info.find(operand->id());
      // operand with multiplicity > 1 selected for overwrite
      if (operand_object_info_it == terms_ctxt_objects_info.end())
      {
        operands_ctxt_objects_ids[i] = term_object_id;
        continue;
      }
      auto &operand_object_info = operand_object_info_it->second;
      operands_ctxt_objects_ids[i] = operand_object_info.id_;
      if (func->data_flow().is_output(operand) || operand->type() == ir::Term::Type::plain || multip > 1)
        continue;

      --operand_object_info.dep_count_;
      if (term_object_id == term->id() && operand_object_info.dep_count_ == 0)
      {
        term_object_id = operands_ctxt_objects_ids[i];
        terms_ctxt_objects_info.erase(operand_object_info_it);
      }
    }

    if (term_object_id == term->id())
    {
      for (auto it = terms_ctxt_objects_info.begin(); it != terms_ctxt_objects_info.end(); ++it)
      {
        if (it->second.dep_count_ == 0)
        {
          term_object_id = it->second.id_;
          terms_ctxt_objects_info.erase(it);
          break;
        }
      }
    }
    auto dep_count = term->parents().size();
    if (func->data_flow().is_output(term))
      ++dep_count;

    terms_ctxt_objects_info.emplace(term->id(), CtxtObjectInfo{term_object_id, dep_count});

    if (term_object_id == term->id())
    {
      os << cipher_type << " ";
      gen_cipher_var_id(term_object_id, os);
      os << ";\n";
    }

    vector<ir::Term::Type> operands_types;
    operands_types.reserve(term->operands().size());
    transform(
      term->operands().cbegin(), term->operands().cend(), back_inserter(operands_types),
      [](const ir::Term *operand) { return operand->type(); });

    if (term->op_code() == ir::OpCode::encrypt)
    {
      os << encryptor_id << ".encrypt(";
      gen_plain_var_id(term->operands()[0]->id(), os);
    }
    else
    {
      os << evaluator_id << "." << operation_mapping.at(ir::OpType{term->op_code().type(), move(operands_types)})
         << "(";
      // operation term needs operands so operands_ctxt_objects_ids cannot be empty
      for (size_t i = 0;; ++i)
      {
        auto operand = term->operands()[i];

        if (operand->type() == ir::Term::Type::cipher)
          gen_cipher_var_id(operands_ctxt_objects_ids[i], os);
        else
          gen_plain_var_id(operand->id(), os);

        if (i == operands_ctxt_objects_ids.size() - 1)
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

void gen_output_terms(
  const ir::OutputTermsInfo &output_terms_info, ostream &os, const TermsCtxtObjectsInfo &terms_ctxt_objects_info)
{
  for (const auto &output_info : output_terms_info)
  {
    auto term = output_info.first;
    if (term->type() == ir::Term::Type::cipher)
    {
      auto ctxt_object_id = terms_ctxt_objects_info.at(term->id()).id_;
      // an output term has at least one label
      for (auto it = output_info.second.labels_.cbegin();;)
      {
        const auto &label = *it;
        os << encrypted_outputs_container_id << ".emplace(\"" << label << "\", ";
        ++it;
        if (it == output_info.second.labels_.cend())
        {
          os << "move(";
          gen_cipher_var_id(ctxt_object_id, os);
          os << "));\n";
          break;
        }
        gen_cipher_var_id(ctxt_object_id, os);
        os << ");\n";
      }
    }
    else
    {
      for (auto it = output_info.second.labels_.cbegin();;)
      {
        const auto &label = *it;
        os << encoded_outputs_container_id << ".emplace(\"" << label << "\", ";
        ++it;
        if (it == output_info.second.labels_.cend())
        {
          os << "move(";
          gen_plain_var_id(term->id(), os);
          os << "));\n";
          break;
        }
        gen_plain_var_id(term->id(), os);
        os << ");\n";
      }
    }
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
