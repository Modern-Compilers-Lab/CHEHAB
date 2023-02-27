#include "seal/seal.h"
#include <chrono>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

using namespace std;
using namespace seal;

vector<vector<chrono::microseconds>> mul_benchmark(SEALContext context, int xdepth, bool mod_switch = false);
void print_parameters(const seal::SEALContext &context);

int main(int argc, char **argv)
{
  int plain_mod_size = 17;
  if (argc > 1)
    plain_mod_size = atoi(argv[1]);

  int xdepth = 10;
  if (argc > 2)
    xdepth = atoi(argv[2]);

  int coeff_mod_data_level_size = 333;
  if (argc > 3)
    coeff_mod_data_level_size = atoi(argv[3]);

  bool mod_switch = false;
  int repeat = 20;

  string id = to_string(plain_mod_size) + "_" + to_string(xdepth) + "_" + to_string(coeff_mod_data_level_size);
  ifstream params_file("params_simulation/saved_params/" + id + ".params", ios::binary);
  EncryptionParameters params;
  params.load(params_file);
  params_file.close();
  SEALContext context(params, true, sec_level_type::tc128);
  print_parameters(context);

  string mod_switch_path = mod_switch ? "with_mod_switch/" : "without_mod_switch/";
  ofstream mul_file;
  mul_file.open("time_logging/" + mod_switch_path + "mul/" + id + ".txt", ios_base::app);
  ofstream relin_file;
  relin_file.open("time_logging/" + mod_switch_path + "relin/" + id + ".txt", ios_base::app);
  ofstream mod_swich_file;
  if (mod_switch)
    mod_swich_file.open("time_logging/with_mod_switch/mod_switch" + id + ".txt", ios_base::app);

  for (int i = 0; i < repeat; ++i)
  {
    vector<vector<chrono::microseconds>> results = mul_benchmark(context, xdepth, mod_switch);

    vector<chrono::microseconds> &time_mul_levels = results[0];
    vector<chrono::microseconds> &time_relin_levels = results[1];
    vector<chrono::microseconds> &time_mod_switch_levels = results[2];

    // Print resutls
    cout << "mul: ";
    for (int i = 0; i < xdepth; ++i)
    {
      mul_file << time_mul_levels[i].count() << " ";
      cout << time_mul_levels[i].count() << " ";
    }
    mul_file << endl;
    cout << endl;

    cout << "relin: ";
    for (int i = 0; i < xdepth; ++i)
    {
      relin_file << time_relin_levels[i].count() << " ";
      cout << time_relin_levels[i].count() << " ";
    }
    relin_file << endl;
    cout << endl;

    if (mod_switch)
    {
      cout << "modswitch: ";
      for (int i = 0; i < xdepth; ++i)
      {
        mod_swich_file << time_mod_switch_levels[i].count() << " ";
        cout << time_mod_switch_levels[i].count() << " ";
      }
      mod_swich_file << endl;
      cout << endl;
    }
    cout << endl;
  }
  mul_file.close();
  relin_file.close();
  if (mod_switch)
    mod_swich_file.close();
  return 0;
}

vector<vector<chrono::microseconds>> mul_benchmark(SEALContext context, int xdepth, bool mod_switch)
{
  chrono::high_resolution_clock::time_point time_start, time_end;
  vector<chrono::microseconds> time_mul_levels(xdepth);
  vector<chrono::microseconds> time_relin_levels(xdepth);
  vector<chrono::microseconds> time_mod_switch_levels(xdepth);

  BatchEncoder batch_encoder(context);
  KeyGenerator keygen(context);
  SecretKey secret_key = keygen.secret_key();
  PublicKey public_key;
  keygen.create_public_key(public_key);
  RelinKeys relin_keys;
  keygen.create_relin_keys(relin_keys);
  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  Decryptor decryptor(context, secret_key);

  size_t slot_count = batch_encoder.slot_count();
  vector<uint64_t> pod_vector;
  random_device rd;
  for (size_t i = 0; i < slot_count; i++)
    pod_vector.push_back(context.first_context_data()->parms().plain_modulus().reduce(rd()));
  Plaintext plain(context.first_context_data()->parms().poly_modulus_degree(), 0);
  batch_encoder.encode(pod_vector, plain);
  Ciphertext encrypted(context);
  encryptor.encrypt(plain, encrypted);

  for (int i = 0; i < xdepth; i++)
  {
    // Multiplication time per level
    time_start = chrono::high_resolution_clock::now();
    evaluator.multiply_inplace(encrypted, encrypted);
    time_end = chrono::high_resolution_clock::now();
    time_mul_levels[i] = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
    // Relinearization time per level
    time_start = chrono::high_resolution_clock::now();
    evaluator.relinearize_inplace(encrypted, relin_keys);
    time_end = chrono::high_resolution_clock::now();
    time_relin_levels[i] = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
    if (mod_switch)
    {
      // Modulus switching time per level
      time_start = chrono::high_resolution_clock::now();
      evaluator.mod_switch_to_next_inplace(encrypted);
      time_end = chrono::high_resolution_clock::now();
      time_mod_switch_levels[i] += chrono::duration_cast<chrono::microseconds>(time_end - time_start);
    }
  }
  if (decryptor.invariant_noise_budget(encrypted) == 0)
    throw logic_error("insufficient noise budget");

  return {time_mul_levels, time_relin_levels, time_mod_switch_levels};
}

void print_parameters(const seal::SEALContext &context)
{
  auto &context_data = *context.key_context_data();

  /*
  Which scheme are we using?
  */
  std::string scheme_name;
  switch (context_data.parms().scheme())
  {
  case seal::scheme_type::bfv:
    scheme_name = "BFV";
    break;
  case seal::scheme_type::ckks:
    scheme_name = "CKKS";
    break;
  case seal::scheme_type::bgv:
    scheme_name = "BGV";
    break;
  default:
    throw std::invalid_argument("unsupported scheme");
  }
  std::cout << "/" << std::endl;
  std::cout << "| Encryption parameters :" << std::endl;
  std::cout << "|   scheme: " << scheme_name << std::endl;
  size_t poly_mod = context_data.parms().poly_modulus_degree();
  std::cout << "|   poly_mod: " << poly_mod << " (" << util::get_significant_bit_count(poly_mod) << " bits)"
            << std::endl;

  /*
  Print the size of the true (product) coefficient modulus.
  */
  std::cout << "|   coeff_mod size: ";
  auto coeff_mod = context_data.parms().coeff_modulus();
  int total_bit_count = context_data.total_coeff_modulus_bit_count();
  std::cout << total_bit_count << " (" << total_bit_count - coeff_mod.back().bit_count() << " + "
            << coeff_mod.back().bit_count() << ") (";
  std::size_t coeff_mod_size = coeff_mod.size();
  for (std::size_t i = 0; i < coeff_mod_size - 1; ++i)
  {
    std::cout << coeff_mod[i].bit_count() << " + ";
  }
  std::cout << coeff_mod.back().bit_count();
  std::cout << ") bits" << std::endl;

  /*
  For BFV and BGV schemes print the plain_mod parameter.
  */
  if (
    context_data.parms().scheme() == seal::scheme_type::bfv || context_data.parms().scheme() == seal::scheme_type::bgv)
  {
    Modulus plain_mod = context_data.parms().plain_modulus();
    std::cout << "|   plain_mod: " << plain_mod.value() << " (" << plain_mod.bit_count() << " bits)" << std::endl;
  }

  std::cout << "\\" << std::endl;
}
