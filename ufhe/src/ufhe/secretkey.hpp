#pragma once

#include "isecretkey.hpp"
#include "seal_backend/secretkey.hpp"

namespace ufhe
{
class SecretKey : public ISecretKey
{
  friend class KeyGenerator;

public:
  inline SecretKey(Backend backend)
  {
    if (backend == Backend::none)
      backend = API::default_backend();
    switch (backend)
    {
    case Backend::seal:
      underlying_ = new seal_backend::SecretKey();
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }
  inline SecretKey(const SecretKey &copy) = delete;

  SecretKey &operator=(const SecretKey &assign) = delete;

  inline ~SecretKey() { delete underlying_; }

  inline Backend backend() override { return underlying().backend(); }

private:
  inline ISecretKey &underlying() const { return *underlying_; }

  ISecretKey *underlying_;
};
} // namespace ufhe
