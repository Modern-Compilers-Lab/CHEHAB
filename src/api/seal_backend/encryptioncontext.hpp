#pragma once

#include "../iencryptioncontext.hpp"
#include "encryptionparameters.hpp"
#include "implementation.hpp"
#include "seal/seal.h"

namespace ufhe
{
namespace seal_backend
{
  class EncryptionContext : public Implementation, public IEncryptionContext
  {
    friend class BatchEncoder;
    friend class KeyGenerator;
    friend class Encryptor;
    friend class Ciphertext;
    friend class Decryptor;
    friend class Evaluator;

  public:
    inline EncryptionContext(const EncryptionParameters &parms) : underlying_(seal::SEALContext(parms.underlying_)) {}

  private:
    seal::SEALContext underlying_;
  };

} // namespace seal_backend
} // namespace ufhe
