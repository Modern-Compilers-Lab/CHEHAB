#pragma once

#include "ufhe/api/modulus.hpp"
#include <memory>

namespace ufhe
{
class Modulus : public api::Modulus
{
  friend class CoeffModulus;
  friend class EncryptionParams;

public:
  Modulus(api::backend_type backend, std::uint64_t value);

  Modulus(std::uint64_t value);

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  int bit_count() const override;

  std::uint64_t value() const override;

  bool is_prime() const override;

  bool operator==(const api::Modulus &compare) const override;

  bool operator!=(const api::Modulus &compare) const override;

  bool operator<(const api::Modulus &compare) const override;

  bool operator<=(const api::Modulus &compare) const override;

  bool operator>(const api::Modulus &compare) const override;

  bool operator>=(const api::Modulus &compare) const override;

  std::uint64_t reduce(std::uint64_t value) const override;

  inline const api::Modulus &underlying() const { return *underlying_; }

private:
  Modulus() = default;

  explicit Modulus(const api::Modulus &modulus);

  std::shared_ptr<api::Modulus> underlying_;
};
} // namespace ufhe
