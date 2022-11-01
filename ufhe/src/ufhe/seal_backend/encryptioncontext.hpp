#pragma once

#include "seal/seal.h"
#include "ufhe/api/iencryptioncontext.hpp"
#include "ufhe/seal_backend/encryptionparameters.hpp"

namespace ufhe
{
namespace seal_backend
{
  class EncryptionContext : public api::IEncryptionContext
  {
    friend class BatchEncoder;
    friend class KeyGenerator;
    friend class Encryptor;
    friend class Ciphertext;
    friend class Decryptor;
    friend class Evaluator;

  public:
    inline EncryptionContext(const EncryptionParameters &parms) : underlying_(seal::SEALContext(parms.underlying_)) {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

  private:
    seal::SEALContext underlying_;
  };

} // namespace seal_backend
} // namespace ufhe
