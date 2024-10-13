#include<iostream>
#include<map>
#include <cstddef>
#include <cstdint>
#include <vector>
#define MOD_BIT_COUNT_MAX 60 
using namespace std ; 
struct NoiseEstimatesValue
  {
    int fresh_noise = 0;
    int mul_noise_growth = 0;
    int mul_plain_noise_growth = 0;
  };

int main(){
  /**
    std::map<int, std::map<std::size_t, NoiseEstimatesValue>> bfv_noise_estimates_seal={
        {14, {{1024, {6, 25, 18}}, {2048, {7, 26, 18}}}},
        {15, {{1024, {6, 25, 18}}}},
        {16, {{1024, {6, 27, 20}}, {2048, {7, 28, 20}}, {4096, {7, 28, 20}}}},
        {17,
            {{1024, {6, 28, 21}},
            {2048, {7, 29, 21}},
            {4096, {7, 30, 22}},
            {8192, {8, 31, 22}},
            {16384, {8, 31, 22}},
            {32768, {8, 32, 22}}}
        },
        {18, {{1024, {6, 29, 22}}, {2048, {7, 30, 22}}, {4096, {7, 30, 22}}, {8192, {7, 31, 23}}, {16384, {8, 32, 23}}}},
        {19, {{1024, {6, 30, 23}}, {2048, {7, 31, 23}}, {4096, {7, 31, 24}}}},
        {20,{{1024, {6, 31, 24}},
            {2048, {7, 32, 25}},
            {4096, {8, 33, 25}},
            {8192, {8, 34, 25}},
            {16384, {8, 35, 25}},
            {32768, {9, 35, 26}}}
        },
    }
  int plain_mod_size = 5 ;
  auto plain_mod_noise_estimates_it = bfv_noise_estimates_seal.find(plain_mod_size);
  while (plain_mod_noise_estimates_it == bfv_noise_estimates_seal.end() &&
         plain_mod_size < bfv_noise_estimates_seal.rbegin()->first && plain_mod_size < MOD_BIT_COUNT_MAX)
  {
    ++plain_mod_size;
    plain_mod_noise_estimates_it = bfv_noise_estimates_seal.find(plain_mod_size);
  }
  /*****************/
  vector<int> coeff_mod_bit_sizes_ ;
  int coeff_mod_data_level_bit_count = 190 ;
  coeff_mod_bit_sizes_.assign(coeff_mod_data_level_bit_count / MOD_BIT_COUNT_MAX, MOD_BIT_COUNT_MAX);
  int remaining_bits = coeff_mod_data_level_bit_count % MOD_BIT_COUNT_MAX;
  if (remaining_bits)
  { 
    coeff_mod_bit_sizes_.push_back(MOD_BIT_COUNT_MAX);
    // Remove exceeding bits
    for (int i = 0; i < MOD_BIT_COUNT_MAX - remaining_bits; ++i)
      --coeff_mod_bit_sizes_.end()[-1 - (i % coeff_mod_bit_sizes_.size())];
  }
  coeff_mod_bit_sizes_.push_back(coeff_mod_bit_sizes_.front());
  for(auto val : coeff_mod_bit_sizes_)
      std::cout<<val<<" ";
  // Consider only data level primes
  int idx=0 ;
  while (idx < coeff_mod_bit_sizes_.size() - 2 && coeff_mod_bit_sizes_[idx] == coeff_mod_bit_sizes_[idx + 1])
    ++idx;
  std::cout<<idx<<" ";
}