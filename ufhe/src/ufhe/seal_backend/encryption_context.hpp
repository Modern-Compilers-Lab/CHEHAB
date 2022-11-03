#pragma once

#include "seal/seal.h"
#include "ufhe/api/encryption_context.hpp"
#include "ufhe/seal_backend/encryption_params.hpp"

namespace ufhe
{
namespace seal_backend
{
  class EncryptionContext : public api::EncryptionContext
  {
    friend class BatchEncoder;
    friend class KeyGenerator;
    friend class Encryptor;
    friend class Ciphertext;
    friend class Decryptor;
    friend class Evaluator;

  public:
    EncryptionContext(const EncryptionParams &params) : underlying_(seal::SEALContext(params.underlying_)) {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

  private:
    seal::SEALContext underlying_;
  };

} // namespace seal_backend
} // namespace ufhe
