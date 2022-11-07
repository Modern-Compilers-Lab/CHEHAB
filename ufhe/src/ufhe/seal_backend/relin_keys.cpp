#include "ufhe/seal_backend/relin_keys.hpp"
#include "seal/relinkeys.h"

namespace ufhe
{
namespace seal_backend
{
  RelinKeys::RelinKeys() : underlying_(std::make_shared<seal::RelinKeys>()) {}

  RelinKeys::RelinKeys(const RelinKeys &copy) : underlying_(std::make_shared<seal::RelinKeys>(copy.underlying())) {}

  RelinKeys &RelinKeys::operator=(const RelinKeys &assign)
  {
    underlying_ = std::make_shared<seal::RelinKeys>(assign.underlying());
    return *this;
  }

  std::size_t RelinKeys::size() const
  {
    return underlying().size();
  }
} // namespace seal_backend
} // namespace ufhe
