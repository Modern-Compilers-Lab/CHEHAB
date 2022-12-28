#pragma once

#include "ufhe/api/decryptor.hpp"
#include <memory>

namespace seal
{
class Decryptor;
} // namespace seal

namespace ufhe
{
namespace seal_backend
{
  class EncryptionContext;
  class SecretKey;

  class Decryptor : public api::Decryptor
  {
  public:
    Decryptor(const EncryptionContext &context, const SecretKey &secret_key);

    Decryptor(const Decryptor &copy) = default;

    Decryptor &operator=(const Decryptor &assign) = default;

    Decryptor(Decryptor &&source) = default;

    Decryptor &operator=(Decryptor &&assign) = default;

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    void decrypt(const api::Ciphertext &encrypted, api::Plaintext &destination) const override;

    int invariant_noise_budget(const api::Ciphertext &encrypted) const override;

    inline const seal::Decryptor &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::Decryptor> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
