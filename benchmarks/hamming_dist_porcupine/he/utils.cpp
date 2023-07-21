#include "utils.hpp"
#include <stdexcept>
#include <utility>

using namespace std;
using namespace seal;

void parse_inputs_outputs_file(
  istream &is, uint64_t plain_modulus, ClearArgsInfo &inputs, ClearArgsInfo &outputs, size_t &slot_count)
{
  ios_base::fmtflags f(is.flags());
  is >> boolalpha;

  size_t nb_inputs, nb_outputs;
  if (!(is >> slot_count >> nb_inputs >> nb_outputs))
    throw invalid_argument("could not parse function general information");

  // parse inputs
  for (size_t i = 0; i < nb_inputs; ++i)
  {
    string var_name;
    bool is_cipher, is_signed;
    if (!(is >> var_name >> is_cipher >> is_signed))
      throw invalid_argument("could not parse input information");

    if (is_signed)
    {
      vector<int64_t> var_value(slot_count);
      for (size_t j = 0; j < slot_count; ++j)
      {
        if (!(is >> var_value[j]))
          throw invalid_argument("could not parse input slot");
      }
      inputs.emplace(var_name, ClearArgInfo{move(var_value), is_cipher, true});
    }
    else
    {
      vector<uint64_t> var_value(slot_count);
      for (size_t j = 0; j < slot_count; ++j)
      {
        if (!(is >> var_value[j]))
          throw invalid_argument("could not parse input slot");
      }
      inputs.emplace(var_name, ClearArgInfo{move(var_value), is_cipher, false});
    }
  }

  // parse outputs
  for (size_t i = 0; i < nb_outputs; ++i)
  {
    string var_name;
    bool is_cipher;
    if (!(is >> var_name >> is_cipher))
      throw invalid_argument("could not parse output information");

    vector<uint64_t> var_value(slot_count);
    for (size_t j = 0; j < slot_count; ++j)
    {
      int64_t slot_value;
      if (!(is >> slot_value))
        throw invalid_argument("could not parse input slot");

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
  is.flags(f);
}

void prepare_he_inputs(
  const BatchEncoder &encoder, const Encryptor &encryptor, const ClearArgsInfo &clear_inputs,
  EncryptedArgs &encrypted_inputs, EncodedArgs &encoded_inputs)
{
  for (const auto &clear_input : clear_inputs)
  {
    Plaintext encoded_input;
    if (clear_input.second.is_signed_)
    {
      const auto &clear_input_value = get<vector<int64_t>>(clear_input.second.value_);
      vector<int64_t> prepared_value(encoder.slot_count());
      for (size_t i = 0; i < prepared_value.size(); ++i)
        prepared_value[i] = clear_input_value[i % clear_input_value.size()];
      encoder.encode(prepared_value, encoded_input);
    }
    else
    {
      const auto &clear_input_value = get<vector<uint64_t>>(clear_input.second.value_);
      vector<uint64_t> prepared_value(encoder.slot_count());
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
  const EncodedArgs &encoded_outputs, size_t slot_count, ClearArgsInfo &clear_outputs)
{
  for (const auto &encrypted_output : encrypted_outputs)
  {
    Plaintext encoded_output;
    decryptor.decrypt(encrypted_output.second, encoded_output);
    vector<uint64_t> clear_output(encoder.slot_count());
    encoder.decode(encoded_output, clear_output);
    clear_output.resize(slot_count);
    clear_outputs.emplace(encrypted_output.first, ClearArgInfo{move(clear_output), true, false});
  }

  for (const auto &encoded_output : encoded_outputs)
  {
    vector<uint64_t> clear_output(encoder.slot_count());
    encoder.decode(encoded_output.second, clear_output);
    clear_output.resize(slot_count);
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
  ios_base::fmtflags f(os.flags());
  os << boolalpha;
  for (const auto &variable : m)
  {
    os << variable.first << " " << variable.second.is_cipher_ << " " << variable.second.is_signed_ << " ";
    if (variable.second.is_signed_)
      print_vec(get<vector<int64_t>>(variable.second.value_), os, print_size);
    else
      print_vec(get<vector<uint64_t>>(variable.second.value_), os, print_size);
    os << '\n';
  }
  os.flags(f);
}

void print_variables_values(const ClearArgsInfo &m, ostream &os)
{
  ios_base::fmtflags f(os.flags());
  os << boolalpha;
  for (const auto &variable : m)
  {
    os << variable.first << " " << variable.second.is_cipher_ << " " << variable.second.is_signed_ << " ";
    if (variable.second.is_signed_)
      print_vec(get<vector<int64_t>>(variable.second.value_), os);
    else
      print_vec(get<vector<uint64_t>>(variable.second.value_), os);
    os << '\n';
  }
  os.flags(f);
}
