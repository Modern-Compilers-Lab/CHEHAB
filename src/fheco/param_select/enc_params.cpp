#include "fheco/param_select/enc_params.hpp"
#include "fheco/util/common.hpp"
#include <cstdint>
#include <utility>

using namespace std;

namespace fheco::param_select
{
EncParams::EncParams(size_t poly_mod_degree, int plain_mod_bit_size, int coeff_mod_data_level_bit_count)
  : poly_mod_degree_{poly_mod_degree}, plain_mod_bit_size_{plain_mod_bit_size}
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

EncParams::EncParams(size_t poly_mod_degree, integer plain_mod, vector<int> coeff_mod_bit_sizes)
  : poly_mod_degree_{poly_mod_degree}, coeff_mod_bit_sizes_{move(coeff_mod_bit_sizes)}
{
  plain_mod_bit_size_ = util::bit_size(static_cast<uint64_t>(plain_mod));
  coeff_mod_bit_count_ = 0;
  for (auto prime_size : coeff_mod_bit_sizes_)
    coeff_mod_bit_count_ += prime_size;
}

int EncParams::increase_coeff_mod_bit_sizes(int max_total_amount)
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

int EncParams::last_coeff_mod_big_prime_idx() const
{
  int idx = 0;
  // Consider only data level primes
  while (idx < coeff_mod_bit_sizes_.size() - 2 && coeff_mod_bit_sizes_[idx] == coeff_mod_bit_sizes_[idx + 1])
    ++idx;
  return idx;
}

vector<int> EncParams::coeff_mod_data_level_bit_sizes() const
{
  vector<int> data_level_part{coeff_mod_bit_sizes_};
  // Remove special prime
  data_level_part.pop_back();
  return data_level_part;
}

void EncParams::print_params(ostream &os) const
{
  os << "poly_mod: " << poly_mod_degree_ << '\n';
  os << "plain_mod_size: " << plain_mod_bit_size_ << " bits" << '\n';
  os << "coeff_mod_size: ";
  os << coeff_mod_bit_count_;
  if (coeff_mod_bit_sizes_.empty())
    return;

  os << " (" << coeff_mod_bit_count_ - coeff_mod_bit_sizes_.back() << " + " << coeff_mod_bit_sizes_.back() << ") (";
  for (auto it = coeff_mod_bit_sizes_.cbegin();;)
  {
    os << *it;
    ++it;
    if (it == coeff_mod_bit_sizes_.cend())
    {
      os << ") bits\n";
      break;
    }
    os << " + ";
  }
}
} // namespace fheco::param_select
