#include "ufhe/seal_backend/ciphertext.hpp"
#include "seal/ciphertext.h"
#include "ufhe/seal_backend/encryption_context.hpp"

namespace ufhe
{
namespace seal_backend
{
  Ciphertext::Ciphertext() : underlying_(std::make_shared<seal::Ciphertext>()) {}

  Ciphertext::Ciphertext(const EncryptionContext &context)
    : underlying_(std::make_shared<seal::Ciphertext>(context.underlying()))
  {}

  Ciphertext::Ciphertext(const Ciphertext &copy) : underlying_(std::make_shared<seal::Ciphertext>(copy.underlying())) {}

  Ciphertext &Ciphertext::operator=(const Ciphertext &assign)
  {
    underlying_ = std::make_shared<seal::Ciphertext>(assign.underlying());
    return *this;
  }

  std::size_t Ciphertext::coeff_modulus_size() const
  {
    return underlying().coeff_modulus_size();
  }

  std::size_t Ciphertext::poly_modulus_degree() const
  {
    return underlying().poly_modulus_degree();
  }

  std::size_t Ciphertext::size() const
  {
    return underlying().size();
  }

  bool Ciphertext::is_transparent() const
  {
    return underlying().is_transparent();
  }

  double &Ciphertext::scale() const
  {
    return underlying_->scale();
  }
} // namespace seal_backend
} // namespace ufhe
