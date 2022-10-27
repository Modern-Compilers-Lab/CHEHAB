#pragma once

#include "../idecryptor.hpp"
#include "ciphertext.hpp"
#include "encryptioncontext.hpp"
#include "implementation.hpp"
#include "plaintext.hpp"
#include "seal/seal.h"
#include "secretkey.hpp"

namespace ufhe
{
namespace seal_backend
{
  class Decryptor : public Implementation, public IDecryptor
  {
  public:
    inline Decryptor(const EncryptionContext &context, const SecretKey &secret_key)
      : Decryptor(new seal::Decryptor(context.underlying(), secret_key.underlying()), true)
    {}

    inline Decryptor(const Decryptor &copy) : Decryptor(copy.underlying_, false) {}

    Decryptor &operator=(const Decryptor &assign) = delete;

    inline ~Decryptor()
    {
      if (is_owner_)
        delete underlying_;
    }

    inline void decrypt(const ICiphertext &encrypted, IPlaintext &destination) const override
    {
      underlying().decrypt(
        dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<Plaintext &>(destination).underlying());
    }

    inline int invariant_noise_budget(const ICiphertext &encrypted) const override
    {
      return underlying().invariant_noise_budget(dynamic_cast<const Ciphertext &>(encrypted).underlying());
    }

  private:
    inline Decryptor(seal::Decryptor *seal_decryptor, bool is_owner) : underlying_(seal_decryptor), is_owner_(is_owner)
    {}

    inline seal::Decryptor &underlying() const { return *underlying_; }

    seal::Decryptor *underlying_;
    bool is_owner_;
  };
} // namespace seal_backend
} // namespace ufhe
