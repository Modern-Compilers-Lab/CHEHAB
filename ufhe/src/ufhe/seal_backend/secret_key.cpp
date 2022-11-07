#include "ufhe/seal_backend/secret_key.hpp"
#include "seal/secretkey.h"

namespace ufhe
{
namespace seal_backend
{
  SecretKey::SecretKey() : underlying_(std::make_shared<seal::SecretKey>()) {}

  SecretKey::SecretKey(const SecretKey &copy) : underlying_(std::make_shared<seal::SecretKey>(copy.underlying())) {}

  SecretKey &SecretKey::operator=(const SecretKey &assign)
  {
    underlying_ = std::make_shared<seal::SecretKey>(assign.underlying());
    return *this;
  }

  SecretKey::SecretKey(const seal::SecretKey &secret_key)
    : underlying_(
        std::shared_ptr<seal::SecretKey>(&const_cast<seal::SecretKey &>(secret_key), [](seal::SecretKey *) {}))
  {}
} // namespace seal_backend
} // namespace ufhe
