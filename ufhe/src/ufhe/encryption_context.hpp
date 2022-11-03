#pragma once

#include "ufhe/api/encryption_context.hpp"
#include "ufhe/config.hpp"
#include "ufhe/encryption_params.hpp"
#include "ufhe/seal_backend/encryption_context.hpp"

namespace ufhe
{
class EncryptionContext : public api::EncryptionContext
{
  friend class KeyGenerator;
  friend class Ciphertext;
  friend class Evaluator;
  friend class BatchEncoder;
  friend class Encryptor;
  friend class Decryptor;

public:
  EncryptionContext(const EncryptionParams &params);

  EncryptionContext(const EncryptionContext &copy) = delete;

  EncryptionContext &operator=(const EncryptionContext &assign) = delete;

  ~EncryptionContext() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

private:
  inline api::EncryptionContext &underlying() const { return *underlying_; }

  api::EncryptionContext *underlying_;
};

} // namespace ufhe
