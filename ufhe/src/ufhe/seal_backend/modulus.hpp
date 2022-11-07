#pragma once

#include "ufhe/api/modulus.hpp"
#include <memory>

namespace seal
{
class Modulus;
} // namespace seal

namespace ufhe
{
class Modulus;

namespace seal_backend
{
  class Modulus : public api::Modulus
  {
    friend class CoeffModulus;
    friend class EncryptionParams;

  public:
    explicit Modulus(std::uint64_t value);

    Modulus(const Modulus &copy) = default;

    Modulus &operator=(const Modulus &assign) = default;

    Modulus(Modulus &&source) = default;

    Modulus &operator=(Modulus &&assign) = default;

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

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

    inline const seal::Modulus &underlying() const { return *underlying_; }

  private:
    explicit Modulus(const seal::Modulus &modulus);

    std::shared_ptr<seal::Modulus> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
