#pragma once

#include "ufhe/api/iencryptioncontext.hpp"
#include "ufhe/config.hpp"
#include "ufhe/encryptionparameters.hpp"
#include "ufhe/seal_backend/encryptioncontext.hpp"

namespace ufhe
{
class EncryptionContext : public api::IEncryptionContext
{
  friend class KeyGenerator;
  friend class Ciphertext;
  friend class Evaluator;
  friend class BatchEncoder;
  friend class Encryptor;
  friend class Decryptor;

public:
  EncryptionContext(const EncryptionParameters &params)
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::EncryptionContext(
        dynamic_cast<const seal_backend::EncryptionParameters &>(params.underlying()));
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  EncryptionContext(const EncryptionContext &copy) = delete;

  EncryptionContext &operator=(const EncryptionContext &assign) = delete;

  ~EncryptionContext() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

private:
  inline IEncryptionContext &underlying() const { return *underlying_; }

  api::IEncryptionContext *underlying_;
};

} // namespace ufhe
