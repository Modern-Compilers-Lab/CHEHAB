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
    inline Ciphertext() : Ciphertext(new seal::Ciphertext(), true) {}

    inline Ciphertext(const EncryptionContext &context) : Ciphertext(new seal::Ciphertext(context.underlying()), true)
    {}

    inline Ciphertext(const Ciphertext &copy) : Ciphertext(copy.underlying_, false) {}

    Ciphertext &operator=(const Ciphertext &assign) = delete;

    inline ~Ciphertext()
    {
      if (is_owner_)
        delete underlying_;
    }

    inline std::size_t coeff_modulus_size() const override { return underlying().coeff_modulus_size(); }

    inline std::size_t poly_modulus_degree() const override { return underlying().poly_modulus_degree(); }

    inline std::size_t size() const override { return underlying().size(); }

    inline bool is_transparent() const override { return underlying().is_transparent(); }

    inline double &scale() override { return underlying().scale(); }

  private:
    inline Ciphertext(seal::Ciphertext *seal_ct, bool is_owner) : underlying_(seal_ct), is_owner_(is_owner) {}

    inline seal::Ciphertext &underlying() const { return *underlying_; }

    seal::Ciphertext *underlying_;
    bool is_owner_;
  };
} // namespace seal_backend
} // namespace ufhe
