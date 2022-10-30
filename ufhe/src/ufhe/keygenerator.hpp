#pragma once

#include "api.hpp"
#include "encryptioncontext.hpp"
#include "ikeygenerator.hpp"
#include "seal_backend/keygenerator.hpp"
#include "secretkey.hpp"

namespace ufhe
{
class KeyGenerator : public IKeyGenerator
{
public:
  inline KeyGenerator(Backend backend, IEncryptionContext &context)
  {
    if (backend == Backend::none)
      backend = API::default_backend();
    switch (backend)
    {
    case Backend::seal:
      underlying_ = new seal_backend::KeyGenerator(dynamic_cast<const EncryptionContext &>(context).underlying());
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  inline KeyGenerator(Backend backend, const IEncryptionContext &context, const ISecretKey &secret_key)
  {
    if (backend == Backend::none)
      backend = API::default_backend();
    switch (backend)
    {
    case Backend::seal:
      underlying_ = new seal_backend::KeyGenerator(
        dynamic_cast<const EncryptionContext &>(context).underlying(),
        dynamic_cast<const SecretKey &>(secret_key).underlying());
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  inline KeyGenerator(const KeyGenerator &copy) = delete;

  KeyGenerator &operator=(const KeyGenerator &assign) = delete;

  inline ~KeyGenerator() { delete underlying_; }

  inline Backend backend() override { return underlying().backend(); }

  inline const ISecretKey &secret_key() const override { return underlying().secret_key(); }

  inline void create_public_key(IPublicKey &destination) const override
  {
    underlying().create_public_key(dynamic_cast<PublicKey &>(destination).underlying());
  }

private:
  inline IKeyGenerator &underlying() const { return *underlying_; }

  IKeyGenerator *underlying_;
};
} // namespace ufhe
