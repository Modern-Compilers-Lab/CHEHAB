#pragma once

#include "ufhe/api/isecretkey.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/secretkey.hpp"

namespace ufhe
{
class SecretKey : public api::ISecretKey
{
  friend class KeyGenerator;
  friend class Encryptor;
  friend class Decryptor;

public:
  SecretKey()
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::SecretKey();
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  SecretKey(const SecretKey &copy)
  {
    switch (copy.backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::SecretKey(dynamic_cast<const seal_backend::SecretKey &>(copy.underlying()));
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  SecretKey &operator=(const SecretKey &assign) = delete;

  ~SecretKey() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

private:
  SecretKey(const api::ISecretKey &isecret_key)
  {
    switch (isecret_key.backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::SecretKey(dynamic_cast<const seal_backend::SecretKey &>(isecret_key));
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  inline ISecretKey &underlying() const { return *underlying_; }

  ISecretKey *underlying_;
};
} // namespace ufhe
