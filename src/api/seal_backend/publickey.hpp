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

  public:
    inline PublicKey() : PublicKey(new seal::PublicKey(), true) {}

    inline PublicKey(const PublicKey &copy) : PublicKey(copy.underlying_, false) {}

    PublicKey &operator=(const PublicKey &assign) = delete;

    inline ~PublicKey()
    {
      if (is_owner_)
        delete underlying_;
    }

  private:
    inline PublicKey(seal::PublicKey *seal_skey, bool is_owner) : underlying_(seal_skey), is_owner_(is_owner) {}

    inline seal::PublicKey &underlying() const { return *underlying_; }

    seal::PublicKey *underlying_;
    bool is_owner_;
  };
} // namespace seal_backend
} // namespace ufhe
