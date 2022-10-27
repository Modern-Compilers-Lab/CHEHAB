#pragma once

#include "../isecretkey.hpp"
#include "seal/seal.h"

namespace ufhe
{
namespace seal_backend
{
  class SecretKey : public ISecretKey
  {
    friend class KeyGenerator;
    friend class Encryptor;
    friend class Decryptor;

  public:
    inline SecretKey() : underlying_(seal::SecretKey()) {}

    inline Backend backend() const override { return Backend::seal; }

  private:
    inline SecretKey(seal::SecretKey seal_sk) : underlying_(seal_sk) {}

    seal::SecretKey underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
