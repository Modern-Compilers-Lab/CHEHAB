#pragma once

#include "seal/seal.h"
#include "ufhe/api/coeff_modulus.hpp"
#include "ufhe/seal_backend/modulus.hpp"
#include <cstddef>
#include <memory>
#include <vector>

namespace ufhe
{
namespace seal_backend
{
  class CoeffModulus : public api::CoeffModulus
  {
    friend class EncryptionParams;

  public:
    CoeffModulus(const std::vector<Modulus> &moduli);

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    api::Modulus::vector value() const override;

    inline const std::vector<seal::Modulus> &underlying() const { return *underlying_; }

  private:
    explicit CoeffModulus(const std::vector<seal::Modulus> &moduli);

    std::shared_ptr<std::vector<seal::Modulus>> underlying_;
    std::vector<Modulus> moduli_;
  };
} // namespace seal_backend
} // namespace ufhe
