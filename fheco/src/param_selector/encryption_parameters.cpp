#include "encryption_parameters.hpp"
#include <iostream>

using namespace std;

namespace param_selector
{
void EncryptionParameters::print_parameters() const
{
  std::cout << "/" << std::endl;
  std::cout << "| Encryption parameters :" << std::endl;
  std::cout << "|   poly_mod: " << poly_modulus_degree_ << std::endl;
  std::cout << "|   plain_mod: " << plain_modulus_bit_size_ << " bits" << std::endl;

  /*
  Print the size of the true (product) coefficient modulus.
  */
  std::cout << "|   coeff_mod size: ";
  std::cout << coeff_mod_bit_count_ << " (" << coeff_mod_bit_count_ - coeff_mod_bit_sizes_.back() << " + "
            << coeff_mod_bit_sizes_.back() << ") (";
  for (std::size_t i = 0; i < coeff_mod_bit_sizes_.size() - 1; ++i)
    std::cout << coeff_mod_bit_sizes_[i] << " + ";
  std::cout << coeff_mod_bit_sizes_.back();
  std::cout << ") bits" << std::endl;
  std::cout << "\\" << std::endl;
}

} // namespace param_selector
