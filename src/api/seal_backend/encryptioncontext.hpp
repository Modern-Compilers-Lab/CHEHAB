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
    friend class KeyGenerator;
    friend class Ciphertext;
    friend class Evaluator;

  public:
    inline EncryptionContext(const EncryptionParameters &parms)
      : EncryptionContext(new seal::SEALContext(parms.underlying()), true)
    {}

    EncryptionContext(const EncryptionContext &copy) : EncryptionContext(copy.underlying_, false) {}

    EncryptionContext &operator=(const EncryptionContext &assign) = delete;

    inline ~EncryptionContext()
    {
      if (is_owner_)
        delete underlying_;
    }

  private:
    EncryptionContext(seal::SEALContext *seal_context, bool is_owner) : underlying_(seal_context), is_owner_(is_owner)
    {}

    inline seal::SEALContext &underlying() const { return *underlying_; }

    seal::SEALContext *underlying_;
    bool is_owner_;
  };

} // namespace seal_backend
} // namespace ufhe
