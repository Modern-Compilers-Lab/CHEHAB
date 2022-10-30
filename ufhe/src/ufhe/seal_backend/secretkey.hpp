#pragma once

#include "seal/seal.h"
#include "ufhe/api/isecretkey.hpp"

namespace ufhe
{
namespace seal_backend
{
  class SecretKey : public api::ISecretKey
  {
    friend class KeyGenerator;
    friend class Encryptor;
    friend class Decryptor;

  public:
    inline SecretKey() : underlying_(seal::SecretKey()) {}

    inline api::BackendType backend() const override { return api::BackendType::seal; }

  private:
    inline SecretKey(seal::SecretKey seal_sk) : underlying_(seal_sk) {}

    seal::SecretKey underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
