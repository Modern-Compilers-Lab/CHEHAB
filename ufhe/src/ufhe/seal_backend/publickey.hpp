#pragma once

#include "seal/seal.h"
#include "ufhe/api/ipublickey.hpp"

namespace ufhe
{
namespace seal_backend
{
  class PublicKey : public api::IPublicKey
  {
    friend class KeyGenerator;
    friend class Encryptor;

  public:
    inline PublicKey() : underlying_(seal::PublicKey()) {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

  private:
    seal::PublicKey underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
