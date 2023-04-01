#include "utils.hpp"
#include <cstddef>
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace seal;

void parse_inputs_outputs_file(const string &file_name, clear_args_info_map &inputs, clear_args_info_map &outputs)
{
  ifstream file(file_name);
  if (!file)
    throw invalid_argument("failed to open file");

  file >> boolalpha;

  size_t vector_size;
  size_t nb_inputs, nb_outputs;
  if (!(file >> vector_size >> nb_inputs >> nb_outputs))
    throw invalid_argument("could not parse function general information");

  // parse inputs
  for (size_t i = 0; i < nb_inputs; ++i)
  {
    string var_name;
    bool is_cipher, is_signed;
    if (!(file >> var_name >> is_cipher >> is_signed))
      throw invalid_argument("could not parse input information");

    if (is_signed)
    {
      vector<int64_t> var_value(vector_size);
      for (size_t j = 0; j < vector_size; ++j)
      {
        if (!(file >> var_value[j]))
          throw invalid_argument("could not parse input slot");
      }
      auto [input_it, inserted] = inputs.insert({var_name, {var_value, is_cipher, true}});
      if (!inserted)
        throw logic_error("repeated input");
    }
    else
    {
      vector<uint64_t> var_value(vector_size);
      for (size_t j = 0; j < vector_size; ++j)
      {
        if (!(file >> var_value[j]))
          throw invalid_argument("could not parse input slot");
      }
      auto [input_it, inserted] = inputs.insert({var_name, {var_value, is_cipher, false}});
      if (!inserted)
        throw logic_error("repeated input");
    }
  }

  // parse outputs
  for (size_t i = 0; i < nb_outputs; ++i)
  {
    string var_name;
    bool is_cipher, is_signed;
    if (!(file >> var_name >> is_cipher >> is_signed))
      throw invalid_argument("could not parse output information");

    if (is_signed)
    {
      vector<int64_t> var_value(vector_size);
      for (size_t j = 0; j < vector_size; ++j)
      {
        if (!(file >> var_value[j]))
          throw invalid_argument("could not parse input slot");
      }
      auto [output_it, inserted] = outputs.insert({var_name, {var_value, is_cipher, true}});
      if (!inserted)
        throw logic_error("repeated output");
    }
    else
    {
      vector<uint64_t> var_value(vector_size);
      for (size_t j = 0; j < vector_size; ++j)
      {
        if (!(file >> var_value[j]))
          throw invalid_argument("could not parse output slot");
      }
      auto [output_it, inserted] = outputs.insert({var_name, {var_value, is_cipher, false}});
      if (!inserted)
        throw logic_error("repeated output");
    }
  }
}

void prepare_he_inputs(
  const BatchEncoder &encoder, const Encryptor &encryptor, const clear_args_info_map &clear_inputs,
  encrypted_args_map &encrypted_inputs, encoded_args_map &encoded_inputs)
{
  for (const auto &clear_input : clear_inputs)
  {
    Plaintext encoded_input;
    if (clear_input.second.is_signed)
    {
      const vector<int64_t> &clear_input_value = get<vector<int64_t>>(clear_input.second.value);
      vector<int64_t> prepared_value(encoder.slot_count());
      for (size_t i = 0; i < prepared_value.size(); ++i)
        prepared_value[i] = clear_input_value[i % clear_input_value.size()];
      encoder.encode(prepared_value, encoded_input);
    }
    else
    {
      const vector<uint64_t> &clear_input_value = get<vector<uint64_t>>(clear_input.second.value);
      vector<uint64_t> prepared_value(encoder.slot_count());
      for (size_t i = 0; i < prepared_value.size(); ++i)
        prepared_value[i] = clear_input_value[i % clear_input_value.size()];
      encoder.encode(prepared_value, encoded_input);
    }

    if (clear_input.second.is_cipher)
    {
      Ciphertext encrypted_input;
      encryptor.encrypt(encoded_input, encrypted_input);
      auto [encrypted_input_it, inserted] = encrypted_inputs.insert({clear_input.first, encrypted_input});
      if (!inserted)
        throw logic_error("ciphertext input already there");
    }
    else
    {
      auto [encoded_input_it, inserted] = encoded_inputs.insert({clear_input.first, encoded_input});
      if (!inserted)
        throw logic_error("plaintext input already there");
    }
  }
}

void get_clear_outputs(
  const BatchEncoder &encoder, Decryptor &decryptor, const encrypted_args_map &encrypted_outputs,
  const encoded_args_map &encoded_outputs, const clear_args_info_map &ref_clear_outputs,
  clear_args_info_map &clear_outputs)
{
  for (const auto &encrypted_output : encrypted_outputs)
  {
    Plaintext encoded_output;
    decryptor.decrypt(encrypted_output.second, encoded_output);
    auto clear_output_info_it = ref_clear_outputs.find(encrypted_output.first);
    if (clear_output_info_it == ref_clear_outputs.end())
      throw invalid_argument("encrypted output without clear info (is_signed)");

    if (clear_output_info_it->second.is_signed)
    {
      vector<int64_t> clear_output;
      encoder.decode(encoded_output, clear_output);
      clear_output.resize(get<vector<int64_t>>(clear_output_info_it->second.value).size());
      auto [clear_output_it, inserted] = clear_outputs.insert({encrypted_output.first, {clear_output, true, true}});
      if (!inserted)
        throw invalid_argument("clear output already there");
    }
    else
    {
      vector<uint64_t> clear_output;
      encoder.decode(encoded_output, clear_output);
      clear_output.resize(get<vector<uint64_t>>(clear_output_info_it->second.value).size());
      auto [clear_output_it, inserted] = clear_outputs.insert({encrypted_output.first, {clear_output, true, false}});
      if (!inserted)
        throw invalid_argument("clear output already there");
    }
  }

  for (const auto &encoded_output : encoded_outputs)
  {
    auto clear_output_info_it = ref_clear_outputs.find(encoded_output.first);
    if (clear_output_info_it == ref_clear_outputs.end())
      throw invalid_argument("encoded output without clear info (is_signed)");

    if (clear_output_info_it->second.is_signed)
    {
      vector<int64_t> clear_output;
      encoder.decode(encoded_output.second, clear_output);
      clear_output.resize(get<vector<int64_t>>(clear_output_info_it->second.value).size());
      auto [clear_output_it, inserted] = clear_outputs.insert({encoded_output.first, {clear_output, true, true}});
      if (!inserted)
        throw invalid_argument("clear output already there");
    }
    else
    {
      vector<uint64_t> clear_output;
      encoder.decode(encoded_output.second, clear_output);
      clear_output.resize(get<vector<uint64_t>>(clear_output_info_it->second.value).size());
      auto [clear_output_it, inserted] = clear_outputs.insert({encoded_output.first, {clear_output, true, false}});
      if (!inserted)
        throw invalid_argument("clear output already there");
    }
  }
}

void print_encrypted_outputs_info(
  const SEALContext &context, Decryptor &decryptor, const encrypted_args_map &encrypted_outputs)
{
  int L = context.first_context_data()->parms().coeff_modulus().size();
  cout << "output ciphertexts info (L=" << L - 1 << ")\n";
  cout << "id: level, remaining_noise_budget, actual_noise_upper_bound (maybe mod_switch was used to sacrifice some "
          "noise budget)\n";
  int init_noise_budget = context.first_context_data()->total_coeff_modulus_bit_count() -
                          context.first_context_data()->parms().plain_modulus().bit_count();
  for (const auto &output : encrypted_outputs)
  {
    int level = context.get_context_data(output.second.parms_id())->chain_index();
    int remaining_noise_budget = decryptor.invariant_noise_budget(output.second);
    int noise_upper_bound = init_noise_budget - remaining_noise_budget;
    cout << output.first << ": " << level << ", " << remaining_noise_budget << ", " << noise_upper_bound << '\n';
  }
}

void print_variables_values(const clear_args_info_map &m, size_t print_size)
{
  std::ios_base::fmtflags f(cout.flags());
  cout << boolalpha;
  for (const auto &variable : m)
  {
    cout << variable.first << " " << variable.second.is_cipher << " " << variable.second.is_signed << " ";
    if (variable.second.is_signed)
      print_vector(get<vector<int64_t>>(variable.second.value), cout, print_size);
    else
      print_vector(get<vector<uint64_t>>(variable.second.value), cout, print_size);
    cout << '\n';
  }
  cout.flags(f);
}

void print_variables_values(const clear_args_info_map &m)
{
  std::ios_base::fmtflags f(cout.flags());
  cout << boolalpha;
  for (const auto &variable : m)
  {
    cout << variable.first << " " << variable.second.is_cipher << " " << variable.second.is_signed << " ";
    if (variable.second.is_signed)
      print_vector(get<vector<int64_t>>(variable.second.value), cout);
    else
      print_vector(get<vector<uint64_t>>(variable.second.value), cout);
    cout << '\n';
  }
  cout.flags(f);
}
