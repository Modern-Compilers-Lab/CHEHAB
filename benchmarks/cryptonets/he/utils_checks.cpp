#include "utils_checks.hpp"
#include <stdexcept>
#include <utility>

using namespace std;
using namespace seal;

void parse_inputs_outputs_file(
  istream &is, uint64_t plain_modulus, ClearArgsInfo &inputs, ClearArgsInfo &outputs, size_t &func_slot_count)
{
  char delim = ' ';
  string header;
  getline(is, header);
  auto tokens = split(header, delim);
  if (tokens.size() < 3)
    throw invalid_argument("malformatted header");

  func_slot_count = stoull(tokens[0]);
  size_t nb_inputs = stoull(tokens[1]);
  size_t nb_outputs = stoull(tokens[2]);

  // parse inputs
  for (size_t i = 0; i < nb_inputs; ++i)
  {
    string line;
    getline(is, line);
    auto tokens = split(line, delim);
    if (tokens.size() < func_slot_count + 3)
      throw invalid_argument("malformatted input line");

    string var_name = tokens[0];
    bool is_cipher = stoi(tokens[1]);
    bool is_signed = stoi(tokens[2]);
    if (is_signed)
    {
      vector<int64_t> var_value(func_slot_count);
      for (size_t j = 0; j < func_slot_count; ++j)
        var_value[j] = stoll(tokens[j + 3]);
      inputs.emplace(var_name, ClearArgInfo{move(var_value), is_cipher, true});
    }
    else
    {
      vector<uint64_t> var_value(func_slot_count);
      for (size_t j = 0; j < func_slot_count; ++j)
        var_value[j] = stoull(tokens[j + 3]);
      inputs.emplace(var_name, ClearArgInfo{move(var_value), is_cipher, false});
    }
  }

  // parse outputs
  for (size_t i = 0; i < nb_outputs; ++i)
  {
    string line;
    getline(is, line);
    auto tokens = split(line, delim);
    if (tokens.size() < func_slot_count + 2)
      throw invalid_argument("malformatted output line");

    string var_name = tokens[0];
    bool is_cipher = stoi(tokens[1]);
    vector<uint64_t> var_value(func_slot_count);
    for (size_t j = 0; j < func_slot_count; ++j)
    {
      int64_t slot_value = stoll(tokens[j + 2]);
      auto signed_plain_modulus = static_cast<int64_t>(plain_modulus);
      if (slot_value >= signed_plain_modulus || slot_value < 0)
      {
        slot_value %= signed_plain_modulus;
        if (slot_value < 0)
          slot_value += signed_plain_modulus;
      }
      var_value[j] = static_cast<uint64_t>(slot_value);
    }
    outputs.emplace(var_name, ClearArgInfo{move(var_value), is_cipher, false});
  }
}

vector<string> split(const string &str, char delim)
{
  vector<string> tokens;
  string token = "";
  for (const auto &c : str)
  {
    if (c == delim)
    {
      tokens.push_back(token);
      token = "";
    }
    else
      token += c;
  }
  tokens.push_back(token);
  return tokens;
}

void prepare_he_inputs(
  const BatchEncoder &encoder, const Encryptor &encryptor, const ClearArgsInfo &clear_inputs,
  EncryptedArgs &encrypted_inputs, EncodedArgs &encoded_inputs)
{
  size_t slot_count = encoder.slot_count();
  for (const auto &clear_input : clear_inputs)
  {
    Plaintext encoded_input;
    if (clear_input.second.is_signed_)
    {
      const auto &clear_input_value = get<vector<int64_t>>(clear_input.second.value_);
      if (clear_input_value.size() > slot_count)
        throw logic_error("input size greater than available slot_count");

      vector<int64_t> prepared_value(slot_count);
      for (size_t i = 0; i < prepared_value.size(); ++i)
        prepared_value[i] = clear_input_value[i % clear_input_value.size()];
      encoder.encode(prepared_value, encoded_input);
    }
    else
    {
      const auto &clear_input_value = get<vector<uint64_t>>(clear_input.second.value_);
      if (clear_input_value.size() > slot_count)
        throw logic_error("input size greater than available slot_count");

      vector<uint64_t> prepared_value(slot_count);
      for (size_t i = 0; i < prepared_value.size(); ++i)
        prepared_value[i] = clear_input_value[i % clear_input_value.size()];
      encoder.encode(prepared_value, encoded_input);
    }

    if (clear_input.second.is_cipher_)
    {
      Ciphertext encrypted_input;
      encryptor.encrypt(encoded_input, encrypted_input);
      encrypted_inputs.emplace(clear_input.first, move(encrypted_input));
    }
    else
      encoded_inputs.emplace(clear_input.first, move(encoded_input));
  }
}

void get_clear_outputs(
  const BatchEncoder &encoder, Decryptor &decryptor, const EncryptedArgs &encrypted_outputs,
  const EncodedArgs &encoded_outputs, size_t func_slot_count, ClearArgsInfo &clear_outputs)
{
  size_t slot_count = encoder.slot_count();
  for (const auto &encrypted_output : encrypted_outputs)
  {
    Plaintext encoded_output;
    decryptor.decrypt(encrypted_output.second, encoded_output);
    vector<uint64_t> clear_output(slot_count);
    encoder.decode(encoded_output, clear_output);
    clear_output.resize(func_slot_count);
    clear_outputs.emplace(encrypted_output.first, ClearArgInfo{move(clear_output), true, false});
  }

  for (const auto &encoded_output : encoded_outputs)
  {
    vector<uint64_t> clear_output(slot_count);
    encoder.decode(encoded_output.second, clear_output);
    clear_output.resize(func_slot_count);
    clear_outputs.emplace(encoded_output.first, ClearArgInfo{move(clear_output), false, false});
  }
}

void print_encrypted_outputs_info(
  const SEALContext &context, Decryptor &decryptor, const EncryptedArgs &encrypted_outputs, ostream &os)
{
  int L = context.first_context_data()->parms().coeff_modulus().size();
  os << "output ciphertexts info (L=" << L - 1 << ")\n";
  os << "id: level, remaining_noise_budget, actual_noise_upper_bound (maybe mod_switch was used to sacrifice some "
        "noise budget)\n";
  int init_noise_budget = context.first_context_data()->total_coeff_modulus_bit_count() -
                          context.first_context_data()->parms().plain_modulus().bit_count();
  for (const auto &output : encrypted_outputs)
  {
    int level = context.get_context_data(output.second.parms_id())->chain_index();
    int remaining_noise_budget = decryptor.invariant_noise_budget(output.second);
    int noise_upper_bound = init_noise_budget - remaining_noise_budget;
    os << output.first << ": " << level << ", " << remaining_noise_budget << ", " << noise_upper_bound << '\n';
  }
}

void print_variables_values(const ClearArgsInfo &m, size_t print_size, ostream &os)
{
  for (const auto &variable : m)
  {
    os << variable.first << " " << variable.second.is_cipher_ << " " << variable.second.is_signed_ << " ";
    if (variable.second.is_signed_)
      print_vec(get<vector<int64_t>>(variable.second.value_), os, print_size);
    else
      print_vec(get<vector<uint64_t>>(variable.second.value_), os, print_size);
    os << '\n';
  }
}

void print_variables_values(const ClearArgsInfo &m, ostream &os)
{
  for (const auto &variable : m)
  {
    os << variable.first << " " << variable.second.is_cipher_ << " " << variable.second.is_signed_ << " ";
    if (variable.second.is_signed_)
      print_vec(get<vector<int64_t>>(variable.second.value_), os);
    else
      print_vec(get<vector<uint64_t>>(variable.second.value_), os);
    os << '\n';
  }
}
