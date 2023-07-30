#include "utils.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
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
    line_data.reserve(tokens.size());
    for (const auto &token : tokens)
      line_data.push_back(stod(token));
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

void export_reduced_weights_biases(
  const Number &modulus, uint64_t plain_modulus, int w1_precis, int w4_precis, int w8_precis, int b1_precis,
  int b4_precis, int b8_precis)
{
  // open weights and biases files
  string raw_data_loc = "data/";
  ifstream w1_is(raw_data_loc + "w1.txt");
  if (!w1_is)
    throw invalid_argument("failed to open w1 file");

  ifstream w4_is(raw_data_loc + "w4.txt");
  if (!w4_is)
    throw invalid_argument("failed to open w4 file");

  ifstream w8_is(raw_data_loc + "w8.txt");
  if (!w8_is)
    throw invalid_argument("failed to open w8 file");

  ifstream b1_is(raw_data_loc + "b1.txt");
  if (!b1_is)
    throw invalid_argument("failed to open b1 file");

  ifstream b4_is(raw_data_loc + "b4.txt");
  if (!b4_is)
    throw invalid_argument("failed to open b4 file");

  ifstream b8_is(raw_data_loc + "b8.txt");
  if (!b8_is)
    throw invalid_argument("failed to open b8 file");

  // load
  char in_delim = ',';
  auto w1_clear = load(w1_is, in_delim);
  auto w4_clear = load(w4_is, in_delim);
  auto w8_clear = load(w8_is, in_delim);
  auto b1_clear = load<double>(b1_is);
  auto b4_clear = load<double>(b4_is);
  auto b8_clear = load<double>(b8_is);
  // compute scaling factors
  const Number w1_scaler = shift(static_cast<Number>(1), w1_precis);
  const Number w4_scaler = shift(static_cast<Number>(1), w4_precis);
  const Number w8_scaler = shift(static_cast<Number>(1), w8_precis);
  const Number b1_scaler = shift(static_cast<Number>(1), b1_precis);
  const Number b4_scaler = shift(static_cast<Number>(1), b4_precis);
  const Number b8_scaler = shift(static_cast<Number>(1), b8_precis);
  // scale
  auto w1_clear_scaled = scale(w1_clear, w1_scaler);
  auto w4_clear_scaled = scale(w4_clear, w4_scaler);
  auto w8_clear_scaled = scale(w8_clear, w8_scaler);
  auto b1_clear_scaled = scale(b1_clear, b1_scaler);
  auto b4_clear_scaled = scale(b4_clear, b4_scaler);
  auto b8_clear_scaled = scale(b8_clear, b8_scaler);
  // reduce
  auto w1_clear_reduced = reduce(w1_clear_scaled, modulus, plain_modulus);
  auto w4_clear_reduced = reduce(w4_clear_scaled, modulus, plain_modulus);
  auto w8_clear_reduced = reduce(w8_clear_scaled, modulus, plain_modulus);
  auto b1_clear_reduced = reduce(b1_clear_scaled, modulus, plain_modulus);
  auto b4_clear_reduced = reduce(b4_clear_scaled, modulus, plain_modulus);
  auto b8_clear_reduced = reduce(b8_clear_scaled, modulus, plain_modulus);
  // print
  string reduced_data_loc = "../constants/plain_mod_" + to_string(plain_modulus) + "/";
  ofstream w1_os(reduced_data_loc + "w1.txt");
  if (!w1_os)
    throw logic_error("failed to create w1 file");

  ofstream w4_os(reduced_data_loc + "w4.txt");
  if (!w4_os)
    throw logic_error("failed to create w4 file");

  ofstream w8_os(reduced_data_loc + "w8.txt");
  if (!w8_os)
    throw logic_error("failed to create w8 file");

  ofstream b1_os(reduced_data_loc + "b1.txt");
  if (!b1_os)
    throw logic_error("failed to create b1 file");

  ofstream b4_os(reduced_data_loc + "b4.txt");
  if (!b4_os)
    throw logic_error("failed to create b4 file");

  ofstream b8_os(reduced_data_loc + "b8.txt");
  if (!b8_os)
    throw logic_error("failed to create b8 file");

  char out_delim = ' ';
  print_reduced_data(w1_clear_reduced, w1_os, out_delim);
  print_reduced_data(w4_clear_reduced, w4_os, out_delim);
  print_reduced_data(w8_clear_reduced, w8_os, out_delim);
  print_reduced_data(b1_clear_reduced, b1_os);
  print_reduced_data(b4_clear_reduced, b4_os);
  print_reduced_data(b8_clear_reduced, b8_os);
}

vector<uint64_t> reduce(const vector<Number> &data, const Number &modulus, uint64_t plain_modulus)
{
  vector<uint64_t> reduced_data(data.size());
  for (size_t i = 0; i < reduced_data.size(); ++i)
  {
    auto val = data[i];
    if (val >= modulus || val < 0)
    {
      val %= modulus;
      if (val < 0)
        val += modulus;
    }
    if (val >= plain_modulus)
      reduced_data[i] = static_cast<uint64_t>(val % plain_modulus);
    else
      reduced_data[i] = static_cast<uint64_t>(val);
  }
  return reduced_data;
}

vector<vector<uint64_t>> reduce(const vector<vector<Number>> &data, const Number &modulus, uint64_t plain_modulus)
{
  vector<vector<uint64_t>> reduced_data(data.size(), vector<uint64_t>(data[0].size()));
  for (size_t i = 0; i < reduced_data.size(); ++i)
  {
    for (size_t j = 0; j < reduced_data[0].size(); ++j)
    {
      auto val = data[i][j];
      if (val >= modulus || val < 0)
      {
        val %= modulus;
        if (val < 0)
          val += modulus;
      }
      if (val >= plain_modulus)
        reduced_data[i][j] = static_cast<uint64_t>(val % plain_modulus);
      else
        reduced_data[i][j] = static_cast<uint64_t>(val);
    }
  }
  return reduced_data;
}

void print_reduced_data(const vector<uint64_t> &data, ostream &os)
{
  for (auto val : data)
    os << val << '\n';
}

void print_reduced_data(const vector<vector<uint64_t>> &data, ostream &os, char delim)
{
  for (const auto &vec : data)
  {
    if (vec.empty())
      continue;

    for (auto it = vec.cbegin();;)
    {
      os << *it;
      ++it;
      if (it == vec.cend())
      {
        os << '\n';
        break;
      }
      os << delim;
    }
  }
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
  const vector<vector<vector<vector<Number>>>> &x, EncryptedArgs &encrypted_inputs)
{
  auto slot_count = encoder.slot_count();
  for (size_t i = 0; i < x.size(); ++i)
  {
    for (size_t j = 0; j < x[0].size(); ++j)
    {
      for (size_t k = 0; k < x[0][0].size(); ++k)
      {
        if (x[i][j][k].size() != slot_count)
          throw logic_error("input size different than available slot_count");

        string input_name = "x[" + to_string(i) + "][" + to_string(j) + "][" + to_string(k) + "]";
        vector<uint64_t> reduced_batched_vals(slot_count);
        for (size_t l = 0; l < slot_count; ++l)
        {
          auto val = x[i][j][k][l];
          if (val >= modulus || val < 0)
          {
            val %= modulus;
            if (val < 0)
              val += modulus;
          }
          if (val >= plain_modulus)
            reduced_batched_vals[l] = static_cast<uint64_t>(val % plain_modulus);
          else
            reduced_batched_vals[l] = static_cast<uint64_t>(val);
        }
        Plaintext ptxt;
        encoder.encode(reduced_batched_vals, ptxt);
        Ciphertext ctxt;
        encryptor.encrypt(ptxt, ctxt);
        encrypted_inputs.emplace(move(input_name), move(ctxt));
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
