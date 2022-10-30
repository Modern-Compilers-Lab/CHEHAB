#pragma once

#include "ipublickey.hpp"
#include "seal_backend/publickey.hpp"

namespace ufhe
{
class PublicKey : public IPublicKey
{
  friend class KeyGenerator;

public:
  inline PublicKey(Backend backend)
  {
    if (backend == Backend::none)
      backend = API::default_backend();
    switch (backend)
    {
    case Backend::seal:
      underlying_ = new seal_backend::PublicKey();
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }
  inline PublicKey(const PublicKey &copy) = delete;

  PublicKey &operator=(const PublicKey &assign) = delete;

  inline ~PublicKey() { delete underlying_; }

  inline Backend backend() override { return underlying().backend(); }

private:
  inline IPublicKey &underlying() const { return *underlying_; }

  IPublicKey *underlying_;
};
} // namespace ufhe
