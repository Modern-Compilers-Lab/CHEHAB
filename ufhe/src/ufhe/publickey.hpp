#pragma once

#include "ufhe/api/ipublickey.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/publickey.hpp"

namespace ufhe
{
class PublicKey : public api::IPublicKey
{
  friend class KeyGenerator;
  friend class Encryptor;

public:
  PublicKey()
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::PublicKey();
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }
  PublicKey(const PublicKey &copy) = delete;

  PublicKey &operator=(const PublicKey &assign) = delete;

  ~PublicKey() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

private:
  inline IPublicKey &underlying() const { return *underlying_; }

  IPublicKey *underlying_;
};
} // namespace ufhe
