#include "../box_blur.hpp"
#include <chrono>
#include <iostream>
#include <vector>

using namespace std;
using namespace seal;

template <typename T>
inline void print_vector(const std::vector<T> &v, std::size_t print_size)
{
  if (v.size() < 2 * print_size)
    throw std::invalid_argument("vector size must at least twice print_size");

  std::size_t size = v.size();
  std::cout << "[";
  for (std::size_t i = 0; i < print_size; i++)
    std::cout << v[i] << ", ";
  if (v.size() > 2 * print_size)
    std::cout << " ..., ";
  for (std::size_t i = size - print_size; i < size; i++)
    std::cout << v[i] << ((i != size - 1) ? ", " : "]\n");
}

int main()
{
  SEALContext context = create_context();

  BatchEncoder batch_encoder(context);
  KeyGenerator keygen(context);
  const SecretKey &secret_key = keygen.secret_key();
  PublicKey public_key;
  keygen.create_public_key(public_key);
  RelinKeys relin_keys;
  keygen.create_relin_keys(relin_keys);
  GaloisKeys galois_keys;
  keygen.create_galois_keys(vector<int>{1, 5}, galois_keys);
  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  Decryptor decryptor(context, secret_key);

  auto &context_data = *context.first_context_data();

  size_t slot_count = batch_encoder.slot_count();
  vector<int64_t> c0_clear = {-12, -61, 1, -22, -54, -64, 38, -5};
  vector<int64_t> c0_clear_prepared(slot_count);
  for (size_t i = 0; i < slot_count; ++i)
    c0_clear_prepared[i] = c0_clear[i % c0_clear.size()];

  Plaintext c0_plain;
  batch_encoder.encode(c0_clear_prepared, c0_plain);
  Ciphertext c0_cipher;
  encryptor.encrypt(c0_plain, c0_cipher);

  unordered_map<string, Ciphertext> encrypted_inputs;
  encrypted_inputs["c0"] = c0_cipher;
  unordered_map<string, Plaintext> encoded_inputs;
  unordered_map<string, Ciphertext> encrypted_outputs;
  unordered_map<string, Plaintext> encoded_outputs;

  int repeat = 1000;

  chrono::high_resolution_clock::time_point time_start, time_end;
  time_start = chrono::high_resolution_clock::now();
  for (int i = 0; i < repeat; ++i)
    box_blur(
      encrypted_inputs, encoded_inputs, encrypted_outputs, encoded_outputs, evaluator, relin_keys, galois_keys,
      public_key);
  time_end = chrono::high_resolution_clock::now();

  chrono::microseconds time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
  cout << endl;
  cout << "exec_time: " << time_diff.count() / repeat << " µs" << endl;

  char c;
  cin >> c;

  int L = context.first_context_data()->parms().coeff_modulus().size();
  cout << "output ciphertexts info (L=" << L << ")" << endl;
  cout << "id: level, remaining_noise_budget, actual_noise_upper_bound (maybe mod_switch was used to sacrifice some "
          "noise budget)"
       << endl;
  int init_noise_budget = context.first_context_data()->total_coeff_modulus_bit_count() -
                          context.first_context_data()->parms().plain_modulus().bit_count();
  for (const auto &output : encrypted_outputs)
  {
    int level = L - context.get_context_data(output.second.parms_id())->chain_index();
    int remaining_noise_budget = decryptor.invariant_noise_budget(output.second);
    int noise_upper_bound = init_noise_budget - remaining_noise_budget;
    cout << output.first << ": " << level << ", " << remaining_noise_budget << ", " << noise_upper_bound << endl;
    Plaintext output_plain;
    decryptor.decrypt(output.second, output_plain);
    vector<int64_t> output_clear;
    batch_encoder.decode(output_plain, output_clear);
    print_vector(output_clear, 4);
  }
}
