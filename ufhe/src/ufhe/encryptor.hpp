#pragma once

#include "ufhe/api/iencryptor.hpp"
#include "ufhe/ciphertext.hpp"
#include "ufhe/encryptioncontext.hpp"
#include "ufhe/plaintext.hpp"
#include "ufhe/publickey.hpp"
#include "ufhe/seal_backend/encryptor.hpp"
#include "ufhe/secretkey.hpp"

namespace ufhe
{
class Encryptor : public api::IEncryptor
{
public:
  Encryptor(const EncryptionContext &context, const PublicKey &public_key)
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::Encryptor(
        dynamic_cast<const seal_backend::EncryptionContext &>(context.underlying()),
        dynamic_cast<const seal_backend::PublicKey &>(public_key.underlying()));
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  inline Encryptor(const EncryptionContext &context, const SecretKey &secret_key)
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::Encryptor(
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

  Encryptor(const EncryptionContext &context, const PublicKey &public_key, const SecretKey &secret_key)
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::Encryptor(
        dynamic_cast<const seal_backend::EncryptionContext &>(context.underlying()),
        dynamic_cast<const seal_backend::PublicKey &>(public_key.underlying()),
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

  Encryptor(const Encryptor &copy) = delete;

  Encryptor &operator=(const Encryptor &assign) = delete;

  ~Encryptor() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline void encrypt(const api::IPlaintext &plain, api::ICiphertext &destination) const override
  {
    underlying().encrypt(
      dynamic_cast<const Plaintext &>(plain).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void encrypt_symmetric(const api::IPlaintext &plain, api::ICiphertext &destination) const override
  {
    underlying().encrypt_symmetric(
      dynamic_cast<const Plaintext &>(plain).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

private:
  inline api::IEncryptor &underlying() const { return *underlying_; }

  api::IEncryptor *underlying_;
};
} // namespace ufhe
