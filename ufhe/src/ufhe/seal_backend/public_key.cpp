#include "ufhe/seal_backend/public_key.hpp"
#include "seal/publickey.h"

namespace ufhe
{
namespace seal_backend
{
  PublicKey::PublicKey() : underlying_(std::make_shared<seal::PublicKey>()) {}

  PublicKey::PublicKey(const PublicKey &copy) : underlying_(std::make_shared<seal::PublicKey>(copy.underlying())) {}

  PublicKey &PublicKey::operator=(const PublicKey &assign)
  {
    underlying_ = std::make_shared<seal::PublicKey>(assign.underlying());
    return *this;
  }
} // namespace seal_backend
} // namespace ufhe
