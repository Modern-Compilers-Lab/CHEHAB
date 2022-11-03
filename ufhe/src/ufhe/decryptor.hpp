#pragma once

#include "ufhe/api/idecryptor.hpp"
#include "ufhe/ciphertext.hpp"
#include "ufhe/encryptioncontext.hpp"
#include "ufhe/plaintext.hpp"
#include "ufhe/seal_backend/decryptor.hpp"
#include "ufhe/secretkey.hpp"

namespace ufhe
{
class Decryptor : public api::IDecryptor
{
public:
  Decryptor(const EncryptionContext &context, const SecretKey &secret_key)
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::Decryptor(
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

  Decryptor(const Decryptor &copy) = delete;

  Decryptor &operator=(const Decryptor &assign) = delete;

  ~Decryptor() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline void decrypt(const api::ICiphertext &encrypted, api::IPlaintext &destination) override
  {
    underlying().decrypt(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<Plaintext &>(destination).underlying());
  }

  inline int invariant_noise_budget(const api::ICiphertext &encrypted) override
  {
    return underlying().invariant_noise_budget(dynamic_cast<const Ciphertext &>(encrypted).underlying());
  }

private:
  inline api::IDecryptor &underlying() const { return *underlying_; }

  api::IDecryptor *underlying_;
};
} // namespace ufhe
