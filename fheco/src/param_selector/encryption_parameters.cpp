#include "encryption_parameters.hpp"
#include <iostream>

using namespace std;

namespace param_selector
{

EncryptionParameters::EncryptionParameters(
  size_t poly_modulus_degree, int plain_modulus_bit_size, int coeff_mod_data_level_bit_count)
  : poly_modulus_degree_(poly_modulus_degree), plain_modulus_bit_size_(plain_modulus_bit_size)
{
  coeff_mod_bit_sizes_.assign(coeff_mod_data_level_bit_count / MOD_BIT_COUNT_MAX, MOD_BIT_COUNT_MAX);
  int remaining_bits = coeff_mod_data_level_bit_count % MOD_BIT_COUNT_MAX;
  if (remaining_bits)
  {
    coeff_mod_bit_sizes_.push_back(MOD_BIT_COUNT_MAX);
    // Remove exceeding bits
    for (int i = 0; i < MOD_BIT_COUNT_MAX - remaining_bits; ++i)
      --coeff_mod_bit_sizes_.end()[-1 - (i % coeff_mod_bit_sizes_.size())];
  }
  // Add special prime, should be as large as the largest of the other primes
  coeff_mod_bit_sizes_.push_back(coeff_mod_bit_sizes_.front());
  coeff_mod_bit_count_ = coeff_mod_data_level_bit_count + coeff_mod_bit_sizes_.back();
}

int EncryptionParameters::increase_coeff_mod_bit_sizes(int max_total_amount)
{

  int start_idx = last_coeff_mod_big_prime_idx() + 1;
  int i;
  for (i = 0; i < max_total_amount; ++i)
  {
    int prime_idx = (start_idx + i) % coeff_mod_bit_sizes_.size();
    if (coeff_mod_bit_sizes_[prime_idx] == MOD_BIT_COUNT_MAX)
      break;

    ++coeff_mod_bit_sizes_[prime_idx];
  }
  coeff_mod_bit_count_ += i;
  return i;
}

int EncryptionParameters::last_coeff_mod_big_prime_idx() const
{
  int idx = 0;
  // Consider only data level primes
  while (idx < coeff_mod_bit_sizes_.size() - 2 && coeff_mod_bit_sizes_[idx] == coeff_mod_bit_sizes_[idx + 1])
    ++idx;
  return idx;
}

vector<int> EncryptionParameters::coeff_mod_data_level_bit_sizes() const
{
  vector<int> data_level_part(coeff_mod_bit_sizes_);
  // Remove special prime
  data_level_part.pop_back();
  return data_level_part;
}

void EncryptionParameters::print_parameters() const
{
  cout << "/" << endl;
  cout << "| Encryption parameters :" << endl;
  cout << "|   poly_mod: " << poly_modulus_degree_ << endl;
  cout << "|   plain_mod: " << plain_modulus_bit_size_ << " bits" << endl;

  /*
  Print the size of the true (product) coefficient modulus.
  */
  cout << "|   coeff_mod size: ";
  cout << coeff_mod_bit_count_ << " (" << coeff_mod_bit_count_ - coeff_mod_bit_sizes_.back() << " + "
       << coeff_mod_bit_sizes_.back() << ") (";
  for (size_t i = 0; i < coeff_mod_bit_sizes_.size() - 1; ++i)
    cout << coeff_mod_bit_sizes_[i] << " + ";
  cout << coeff_mod_bit_sizes_.back();
  cout << ") bits" << endl;
  cout << "\\" << endl;
}

} // namespace param_selector
