#include "ufhe/seal_backend/galois_keys.hpp"
#include "seal/galoiskeys.h"

namespace ufhe
{
namespace seal_backend
{
  GaloisKeys::GaloisKeys() : underlying_(std::make_shared<seal::GaloisKeys>()) {}

  GaloisKeys::GaloisKeys(const GaloisKeys &copy) : underlying_(std::make_shared<seal::GaloisKeys>(copy.underlying())) {}

  GaloisKeys &GaloisKeys::operator=(const GaloisKeys &assign)
  {
    underlying_ = std::make_shared<seal::GaloisKeys>(assign.underlying());
    return *this;
  }

  std::size_t GaloisKeys::size() const
  {
    return underlying().size();
  }
} // namespace seal_backend
} // namespace ufhe
