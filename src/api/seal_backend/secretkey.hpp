#pragma once

#include "../isecretkey.hpp"
#include "implementation.hpp"
#include "seal/seal.h"

namespace ufhe
{
namespace seal_backend
{
  class SecretKey : public Implementation, public ISecretKey
  {
    friend class KeyGenerator;
    friend class Encryptor;
    friend class Decryptor;

  public:
    inline SecretKey() : SecretKey(new seal::SecretKey(), true) {}

    inline SecretKey(const SecretKey &copy) : SecretKey(copy.underlying_, false) {}

    SecretKey &operator=(const SecretKey &assign) = delete;

    inline ~SecretKey()
    {
      if (is_owner_)
        delete underlying_;
    }

  private:
    inline SecretKey(seal::SecretKey *seal_skey, bool is_owner) : underlying_(seal_skey), is_owner_(is_owner) {}

    inline seal::SecretKey &underlying() const { return *underlying_; }

    seal::SecretKey *underlying_;
    bool is_owner_;
  };
} // namespace seal_backend
} // namespace ufhe
