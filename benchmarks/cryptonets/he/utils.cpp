#include "utils.hpp"
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>

using namespace std;
using namespace seal;

vector<vector<double>> load(istream &is, char delim)
{
  vector<vector<double>> data;
  string line;
  while (getline(is, line))
  {
    auto tokens = split(line, delim);
    vector<double> line_data;
    for (const auto &token : tokens)
      line_data.push_back(stod(trim(token)));
    data.push_back(line_data);
  }
  return data;
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

string trim(string s)
{
  s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) { return !isspace(ch); }));
  s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !isspace(ch); }).base(), s.end());
  return s;
}

vector<vector<vector<vector<double>>>> reshape_4d(const vector<vector<double>> &data, const vector<size_t> &shape)
{
  if (shape.size() < 4)
    throw invalid_argument("incomplete dimensions");

  vector<vector<vector<vector<double>>>> result(
    shape[0], vector<vector<vector<double>>>(shape[1], vector<vector<double>>(shape[2], vector<double>(shape[3]))));
  size_t i, j, k, l;
  i = j = k = l = 0;
  bool full = false;
  for (const auto &vec : data)
  {
    for (const auto &e : vec)
    {
      result[i][j][k][l] = e;
      ++l;
      if (l == shape[3])
      {
        l = 0;
        ++k;
        if (k == shape[2])
        {
          k = 0;
          ++j;
          if (j == shape[1])
          {
            j = 0;
            ++i;
            if (i == shape[0])
            {
              full = true;
              break;
            }
          }
        }
      }
    }
    if (full)
      break;
  }
  return result;
}

Number scale(double n, const Number &scaler)
{
  return n * scaler;
}

vector<Number> scale(const vector<double> &data, const Number &scaler)
{
  vector<Number> result;
  result.reserve(data.size());
  for (auto e : data)
    result.push_back(scale(e, scaler));
  return result;
}

vector<vector<Number>> scale(const vector<vector<double>> &data, const Number &scaler)
{
  vector<vector<Number>> result;
  result.reserve(data.size());
  for (const auto &e : data)
    result.push_back(scale(e, scaler));
  return result;
}

vector<vector<vector<Number>>> scale(const vector<vector<vector<double>>> &data, const Number &scaler)
{
  vector<vector<vector<Number>>> result;
  result.reserve(data.size());
  for (const auto &e : data)
    result.push_back(scale(e, scaler));
  return result;
}

vector<vector<vector<vector<Number>>>> scale(const vector<vector<vector<vector<double>>>> &data, const Number &scaler)
{
  vector<vector<vector<vector<Number>>>> result;
  result.reserve(data.size());
  for (const auto &e : data)
    result.push_back(scale(e, scaler));
  return result;
}

vector<vector<Number>> reshape_order(const vector<vector<Number>> &data, const vector<size_t> &order)
{
  if (order.size() != 2)
    throw invalid_argument("order must be of size 2");

  vector<size_t> shape = {data.size(), data[0].size()};
  vector<size_t> new_shape;
  new_shape.reserve(2);
  for (auto e : order)
    new_shape.push_back(shape[e]);
  vector<vector<Number>> r(new_shape[0], vector<Number>(new_shape[1]));
  vector<size_t> i = {0, 0, 0};
  for (i[0] = 0; i[0] < shape[0]; ++i[0])
    for (i[1] = 0; i[1] < shape[1]; ++i[1])
      r[i[order[0]]][i[order[1]]] = data[i[0]][i[1]];
  return r;
}

vector<vector<vector<Number>>> reshape_order(const vector<vector<vector<Number>>> &data, const vector<size_t> &order)
{
  if (order.size() != 3)
    throw invalid_argument("order must be of size 3");

  vector<size_t> shape = {data.size(), data[0].size(), data[0][0].size()};
  vector<size_t> new_shape;
  new_shape.reserve(3);
  for (auto e : order)
    new_shape.push_back(shape[e]);
  vector<vector<vector<Number>>> r(new_shape[0], vector<vector<Number>>(new_shape[1], vector<Number>(new_shape[2])));
  vector<size_t> i = {0, 0, 0};
  for (i[0] = 0; i[0] < shape[0]; ++i[0])
    for (i[1] = 0; i[1] < shape[1]; ++i[1])
      for (i[2] = 0; i[2] < shape[2]; ++i[2])
        r[i[order[0]]][i[order[1]]][i[order[2]]] = data[i[0]][i[1]][i[2]];
  return r;
}

vector<vector<vector<vector<Number>>>> reshape_order(
  const vector<vector<vector<vector<Number>>>> &data, const vector<size_t> &order)
{
  if (order.size() != 4)
    throw invalid_argument("order must be of size 4");

  vector<size_t> shape = {data.size(), data[0].size(), data[0][0].size(), data[0][0][0].size()};
  vector<size_t> new_shape;
  new_shape.reserve(4);
  for (auto e : order)
    new_shape.push_back(shape[e]);
  vector<vector<vector<vector<Number>>>> result(
    new_shape[0],
    vector<vector<vector<Number>>>(new_shape[1], vector<vector<Number>>(new_shape[2], vector<Number>(new_shape[3]))));
  vector<size_t> i = {0, 0, 0, 0};
  for (i[0] = 0; i[0] < shape[0]; ++i[0])
    for (i[1] = 0; i[1] < shape[1]; ++i[1])
      for (i[2] = 0; i[2] < shape[2]; ++i[2])
        for (i[3] = 0; i[3] < shape[3]; ++i[3])
          result[i[order[0]]][i[order[1]]][i[order[2]]][i[order[3]]] = data[i[0]][i[1]][i[2]][i[3]];
  return result;
}

void prepare_he_inputs(
  const Number &modulus, uint64_t plain_modulus, const BatchEncoder &encoder, const Encryptor &encryptor,
  const vector<vector<vector<vector<Number>>>> &x, const vector<vector<vector<vector<Number>>>> &w1,
  const vector<vector<vector<vector<Number>>>> &w4, const vector<vector<Number>> &w8, const vector<Number> &b1,
  const vector<Number> &b4, const vector<Number> &b8, EncryptedArgs &encrypted_inputs, EncodedArgs &encoded_inputs)
{
  prepare_inputs_group(
    modulus, plain_modulus, encoder, encryptor, x, "x", true, true, encrypted_inputs, encoded_inputs);
  prepare_inputs_group(
    modulus, plain_modulus, encoder, encryptor, w1, "w1", false, false, encrypted_inputs, encoded_inputs);
  prepare_inputs_group(
    modulus, plain_modulus, encoder, encryptor, w4, "w4", false, false, encrypted_inputs, encoded_inputs);
  prepare_inputs_group(
    modulus, plain_modulus, encoder, encryptor, w8, "w8", false, false, encrypted_inputs, encoded_inputs);
  prepare_inputs_group(
    modulus, plain_modulus, encoder, encryptor, b1, "b1", false, false, encrypted_inputs, encoded_inputs);
  prepare_inputs_group(
    modulus, plain_modulus, encoder, encryptor, b4, "b4", false, false, encrypted_inputs, encoded_inputs);
  prepare_inputs_group(
    modulus, plain_modulus, encoder, encryptor, b8, "b8", false, false, encrypted_inputs, encoded_inputs);
}

void prepare_inputs_group(
  const Number &modulus, uint64_t plain_modulus, const BatchEncoder &encoder, const Encryptor &encryptor,
  const vector<Number> &inputs, const string &group_name, bool encrypt, bool pack, EncryptedArgs &encrypted_inputs,
  EncodedArgs &encoded_inputs)
{
  auto slot_count = encoder.slot_count();
  if (pack)
  {
    string input_name = group_name;
    vector<Number> batched_vals(slot_count);
    if (inputs.size() != slot_count)
      throw logic_error("inputs size different than available slot_count");

    for (size_t i = 0; i < slot_count; ++i)
    {
      auto slot_val = inputs[i];
      if (slot_val >= modulus || slot_val < 0)
      {
        slot_val %= modulus;
        if (slot_val < 0)
          slot_val += modulus;
      }
      batched_vals[i] = slot_val;
    }
    vector<uint64_t> reduced_batched_vals(slot_count);
    for (size_t i = 0; i < slot_count; ++i)
      reduced_batched_vals[i] = static_cast<uint64_t>(batched_vals[i] % plain_modulus);
    Plaintext ptxt;
    encoder.encode(reduced_batched_vals, ptxt);
    if (encrypt)
    {
      Ciphertext ctxt;
      encryptor.encrypt(ptxt, ctxt);
      encrypted_inputs.emplace(move(input_name), move(ctxt));
    }
    else
      encoded_inputs.emplace(move(input_name), move(ptxt));
  }
  else
  {
    for (size_t i = 0; i < inputs.size(); ++i)
    {
      string input_name = group_name + "[" + to_string(i) + "]";
      Number val = inputs[i];
      if (val >= modulus || val < 0)
      {
        val %= modulus;
        if (val < 0)
          val += modulus;
      }
      vector<Number> batched_vals(slot_count, val);
      uint64_t reduced_val = static_cast<uint64_t>(val % plain_modulus);
      vector<uint64_t> reduced_batched_vals(slot_count, reduced_val);
      Plaintext ptxt;
      encoder.encode(reduced_batched_vals, ptxt);
      if (encrypt)
      {
        Ciphertext ctxt;
        encryptor.encrypt(ptxt, ctxt);
        encrypted_inputs.emplace(move(input_name), move(ctxt));
      }
      else
        encoded_inputs.emplace(move(input_name), move(ptxt));
    }
  }
}

void prepare_inputs_group(
  const Number &modulus, uint64_t plain_modulus, const BatchEncoder &encoder, const Encryptor &encryptor,
  const vector<vector<Number>> &inputs, const string &group_name, bool encrypt, bool pack,
  EncryptedArgs &encrypted_inputs, EncodedArgs &encoded_inputs)
{
  auto slot_count = encoder.slot_count();
  for (size_t i = 0; i < inputs.size(); ++i)
  {
    if (pack)
    {
      string input_name = group_name + "[" + to_string(i) + "]";
      vector<Number> batched_vals(slot_count);
      if (inputs[0].size() != slot_count)
        throw logic_error("inputs size different than available slot_count");

      for (size_t j = 0; j < slot_count; ++j)
      {
        auto slot_val = inputs[i][j];
        if (slot_val >= modulus || slot_val < 0)
        {
          slot_val %= modulus;
          if (slot_val < 0)
            slot_val += modulus;
        }
        batched_vals[j] = slot_val;
      }
      vector<uint64_t> reduced_batched_vals(slot_count);
      for (size_t j = 0; j < slot_count; ++j)
        reduced_batched_vals[j] = static_cast<uint64_t>(batched_vals[j] % plain_modulus);
      Plaintext ptxt;
      encoder.encode(reduced_batched_vals, ptxt);
      if (encrypt)
      {
        Ciphertext ctxt;
        encryptor.encrypt(ptxt, ctxt);
        encrypted_inputs.emplace(move(input_name), move(ctxt));
      }
      else
        encoded_inputs.emplace(move(input_name), move(ptxt));
    }
    else
    {
      for (size_t j = 0; j < inputs[0].size(); ++j)
      {
        string input_name = group_name + "[" + to_string(i) + "][" + to_string(j) + "]";
        Number val = inputs[i][j];
        if (val >= modulus || val < 0)
        {
          val %= modulus;
          if (val < 0)
            val += modulus;
        }
        vector<Number> batched_vals(slot_count, val);
        uint64_t reduced_val = static_cast<uint64_t>(val % plain_modulus);
        vector<uint64_t> reduced_batched_vals(slot_count, reduced_val);
        Plaintext ptxt;
        encoder.encode(reduced_batched_vals, ptxt);
        if (encrypt)
        {
          Ciphertext ctxt;
          encryptor.encrypt(ptxt, ctxt);
          encrypted_inputs.emplace(move(input_name), move(ctxt));
        }
        else
          encoded_inputs.emplace(move(input_name), move(ptxt));
      }
    }
  }
}

void prepare_inputs_group(
  const Number &modulus, uint64_t plain_modulus, const BatchEncoder &encoder, const Encryptor &encryptor,
  const vector<vector<vector<vector<Number>>>> &inputs, const string &group_name, bool encrypt, bool pack,
  EncryptedArgs &encrypted_inputs, EncodedArgs &encoded_inputs)
{
  auto slot_count = encoder.slot_count();
  for (size_t i = 0; i < inputs.size(); ++i)
  {
    for (size_t j = 0; j < inputs[0].size(); ++j)
    {
      for (size_t k = 0; k < inputs[0][0].size(); ++k)
      {
        if (pack)
        {
          string input_name = group_name + "[" + to_string(i) + "][" + to_string(j) + "][" + to_string(k) + "]";
          vector<Number> batched_vals(slot_count);
          if (inputs[0][0][0].size() != slot_count)
            throw logic_error("inputs size different than available slot_count");

          for (size_t l = 0; l < slot_count; ++l)
          {
            auto val = inputs[i][j][k][l];
            if (val >= modulus || val < 0)
            {
              val %= modulus;
              if (val < 0)
                val += modulus;
            }
            batched_vals[l] = val;
          }
          vector<uint64_t> reduced_batched_vals(slot_count);
          for (size_t l = 0; l < slot_count; ++l)
            reduced_batched_vals[l] = static_cast<uint64_t>(batched_vals[l] % plain_modulus);
          Plaintext ptxt;
          encoder.encode(reduced_batched_vals, ptxt);
          if (encrypt)
          {
            Ciphertext ctxt;
            encryptor.encrypt(ptxt, ctxt);
            encrypted_inputs.emplace(move(input_name), move(ctxt));
          }
          else
            encoded_inputs.emplace(move(input_name), move(ptxt));
        }
        else
        {
          for (size_t l = 0; l < inputs[0][0][0].size(); ++l)
          {
            string input_name =
              group_name + "[" + to_string(i) + "][" + to_string(j) + "][" + to_string(k) + "][" + to_string(l) + "]";
            Number val = inputs[i][j][k][l];
            if (val >= modulus || val < 0)
            {
              val %= modulus;
              if (val < 0)
                val += modulus;
            }
            vector<Number> batched_vals(slot_count, val);
            uint64_t reduced_val = static_cast<uint64_t>(val % plain_modulus);
            vector<uint64_t> reduced_batched_vals(slot_count, reduced_val);
            Plaintext ptxt;
            encoder.encode(reduced_batched_vals, ptxt);
            if (encrypt)
            {
              Ciphertext ctxt;
              encryptor.encrypt(ptxt, ctxt);
              encrypted_inputs.emplace(move(input_name), move(ctxt));
            }
            else
              encoded_inputs.emplace(move(input_name), move(ptxt));
          }
        }
      }
    }
  }
}

vector<vector<Number>> get_clear_outputs(
  const vector<uint64_t> &coprimes, const Number &modulus, const vector<unique_ptr<BatchEncoder>> &encoders,
  vector<unique_ptr<Decryptor>> &decryptors, const vector<EncryptedArgs> &primes_encrypted_outputs)
{
  vector<Number> nx;
  nx.reserve(coprimes.size());
  for (auto prime : coprimes)
  {
    Number ni = modulus / prime;
    Number nxi;
    for (uint64_t xi = 0; (nxi = xi * ni) % prime != 1; ++xi)
      ;
    nx.push_back(nxi);
  }

  size_t outputs_count = primes_encrypted_outputs[0].size();
  vector<vector<Plaintext>> outputs_primes_ptxts(outputs_count, vector<Plaintext>(coprimes.size()));
  for (size_t i_out = 0; i_out < outputs_count; ++i_out)
  {
    for (size_t i_prime = 0; i_prime < coprimes.size(); ++i_prime)
    {
      const auto &ctxt = primes_encrypted_outputs[i_prime].at("y[" + to_string(i_out) + "]");
      decryptors[i_prime]->decrypt(ctxt, outputs_primes_ptxts[i_out][i_prime]);
    }
  }

  size_t slot_count = encoders[0]->slot_count();
  vector<vector<vector<uint64_t>>> outputs_primes_clear_data(
    outputs_count, vector<vector<uint64_t>>(coprimes.size(), vector<uint64_t>(slot_count)));
  for (size_t i_out = 0; i_out < outputs_count; ++i_out)
  {
    for (size_t i_prime = 0; i_prime < coprimes.size(); ++i_prime)
      encoders[i_prime]->decode(outputs_primes_ptxts[i_out][i_prime], outputs_primes_clear_data[i_out][i_prime]);
  }

  vector<vector<vector<Number>>> outputs_clear_data_primes(
    outputs_count, vector<vector<Number>>(slot_count, vector<Number>(coprimes.size())));
  for (size_t i_out = 0; i_out < outputs_count; ++i_out)
  {
    for (size_t i_elem = 0; i_elem < slot_count; ++i_elem)
    {
      for (size_t i_prime = 0; i_prime < coprimes.size(); ++i_prime)
        outputs_clear_data_primes[i_out][i_elem][i_prime] =
          outputs_primes_clear_data[i_out][i_prime][i_elem] * nx[i_prime] % modulus;
    }
  }

  bool is_signed = true;
  vector<vector<Number>> outputs_clear_data(outputs_count, vector<Number>(slot_count, 0));
  for (size_t i_out = 0; i_out < outputs_count; ++i_out)
  {
    for (size_t i_elem = 0; i_elem < slot_count; ++i_elem)
    {
      for (size_t i_prime = 0; i_prime < coprimes.size(); ++i_prime)
        outputs_clear_data[i_out][i_elem] += outputs_clear_data_primes[i_out][i_elem][i_prime];

      outputs_clear_data[i_out][i_elem] %= modulus;
      if (is_signed && outputs_clear_data[i_out][i_elem] > modulus / 2)
        outputs_clear_data[i_out][i_elem] -= modulus;
    }
  }

  return outputs_clear_data;
}

vector<size_t> argmax(const vector<vector<Number>> &data)
{
  vector<size_t> result;
  result.reserve(data.size());
  for (size_t i = 0; i < data.size(); ++i)
  {
    size_t idx = 0;
    for (size_t j = 0; j < data[0].size(); ++j)
      if (data[i][j] > data[i][idx])
        idx = j;
    result.push_back(idx);
  }
  return result;
}

size_t count_equal(const vector<uint64_t> &a, const vector<uint64_t> &b)
{
  if (a.size() != b.size())
    throw invalid_argument("vectors must have the same size");

  size_t count = 0;
  for (size_t i = 0; i < a.size(); ++i)
    count += a[i] == b[i];
  return count;
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
