#pragma once

#include "../ipublickey.hpp"
#include "seal/seal.h"

namespace ufhe
{
namespace seal_backend
{
  class PublicKey : public IPublicKey
  {
    friend class KeyGenerator;
    friend class Encryptor;

  public:
    inline PublicKey() : underlying_(seal::PublicKey()) {}

    inline Backend backend() const override { return Backend::seal; }

  private:
    seal::PublicKey underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
