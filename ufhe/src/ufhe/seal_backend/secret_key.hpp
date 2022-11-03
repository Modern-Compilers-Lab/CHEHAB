#pragma once

#include "seal/seal.h"
#include "ufhe/api/secret_key.hpp"

namespace ufhe
{
namespace seal_backend
{
  class SecretKey : public api::SecretKey
  {
    friend class KeyGenerator;
    friend class Encryptor;
    friend class Decryptor;

  public:
    SecretKey() : underlying_(seal::SecretKey()) {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

  private:
    SecretKey(seal::SecretKey seal_sk) : underlying_(seal_sk) {}

    seal::SecretKey underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
