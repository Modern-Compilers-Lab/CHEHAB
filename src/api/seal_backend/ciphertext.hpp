#pragma once

#include "../iciphertext.hpp"
#include "encryptioncontext.hpp"
#include "implementation.hpp"
#include "seal/seal.h"

namespace ufhe
{
namespace seal_backend
{
  class Ciphertext : public Implementation, public ICiphertext
  {
    friend class Evaluator;
    friend class Encryptor;
    friend class Decryptor;

  public:
    inline Ciphertext() : underlying_(seal::Ciphertext()) {}

    inline Ciphertext(const EncryptionContext &context) : underlying_(seal::Ciphertext(context.underlying_)) {}

    inline std::size_t coeff_modulus_size() const override { return underlying_.coeff_modulus_size(); }

    inline std::size_t poly_modulus_degree() const override { return underlying_.poly_modulus_degree(); }

    inline std::size_t size() const override { return underlying_.size(); }

    inline bool is_transparent() const override { return underlying_.is_transparent(); }

    inline double &scale() override { return underlying_.scale(); }

  private:
    seal::Ciphertext underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
