#pragma once

#include "ufhe/api/encryptor.hpp"
#include <memory>

namespace seal
{
class Encryptor;
} // namespace seal

namespace ufhe
{
namespace seal_backend
{
  class EncryptionContext;
  class PublicKey;
  class SecretKey;

  class Encryptor : public api::Encryptor
  {
  public:
    Encryptor(const EncryptionContext &context, const PublicKey &public_key);

    Encryptor(const EncryptionContext &context, const SecretKey &secret_key);

    Encryptor(const EncryptionContext &context, const PublicKey &public_key, const SecretKey &secret_key);

    Encryptor(const Encryptor &copy) = default;

    Encryptor &operator=(const Encryptor &assign) = default;

    Encryptor(Encryptor &&source) = default;

    Encryptor &operator=(Encryptor &&assign) = default;

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    void encrypt(const api::Plaintext &plain, api::Ciphertext &destination) const override;

    void encrypt_symmetric(const api::Plaintext &plain, api::Ciphertext &destination) const override;

    inline const seal::Encryptor &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::Encryptor> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
