#pragma once

#include "seal_backend/keygenerator.hpp"
#include "ufhe/api/ikeygenerator.hpp"
#include "ufhe/encryptioncontext.hpp"
#include "ufhe/publickey.hpp"
#include "ufhe/secretkey.hpp"

namespace ufhe
{
class KeyGenerator : public api::IKeyGenerator
{
public:
  KeyGenerator(EncryptionContext &context)
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ =
        new seal_backend::KeyGenerator(dynamic_cast<const seal_backend::EncryptionContext &>(context.underlying()));
      secret_key_ = new SecretKey(underlying().secret_key());
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  KeyGenerator(const EncryptionContext &context, const SecretKey &secret_key)
  {
    secret_key_ = new SecretKey(secret_key);
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::KeyGenerator(
        dynamic_cast<const seal_backend::EncryptionContext &>(context.underlying()),
        dynamic_cast<const seal_backend::SecretKey &>(secret_key.underlying()));
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  KeyGenerator(const KeyGenerator &copy) = delete;

  KeyGenerator &operator=(const KeyGenerator &assign) = delete;

  ~KeyGenerator()
  {
    delete underlying_;
    delete secret_key_;
  }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline const SecretKey &secret_key() const override { return *secret_key_; }

  inline void create_public_key(api::IPublicKey &destination) const override
  {
    underlying().create_public_key(dynamic_cast<PublicKey &>(destination).underlying());
  }

private:
  inline IKeyGenerator &underlying() const { return *underlying_; }

  IKeyGenerator *underlying_;
  SecretKey *secret_key_;
};
} // namespace ufhe
