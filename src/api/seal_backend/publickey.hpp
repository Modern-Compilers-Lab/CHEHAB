#pragma once

#include "../ipublickey.hpp"
#include "implementation.hpp"
#include "seal/seal.h"

namespace ufhe
{
namespace seal_backend
{
  class PublicKey : public Implementation, public IPublicKey
  {
    friend class KeyGenerator;
    friend class Encryptor;

  public:
    inline PublicKey() : underlying_(seal::PublicKey()) {}

  private:
    seal::PublicKey underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
