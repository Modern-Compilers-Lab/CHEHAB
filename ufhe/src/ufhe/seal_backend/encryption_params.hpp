#pragma once

#include "ufhe/api/encryption_params.hpp"
#include "ufhe/seal_backend/coeff_modulus.hpp"
#include "ufhe/seal_backend/modulus.hpp"
#include "ufhe/seal_backend/scheme.hpp"
#include <memory>

namespace seal
{
class EncryptionParameters;
} // namespace seal

namespace ufhe
{
namespace seal_backend
{
  class EncryptionParams : public api::EncryptionParams
  {
  public:
    explicit EncryptionParams(const Scheme &scheme);

    EncryptionParams(const EncryptionParams &copy);

    EncryptionParams &operator=(const EncryptionParams &assign);

    EncryptionParams(EncryptionParams &&source) = default;

    EncryptionParams &operator=(EncryptionParams &&assign) = default;

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    void set_poly_modulus_degree(std::size_t poly_modulus_degree) override;

    void set_coeff_modulus(const api::CoeffModulus &coeff_modulus) override;

    void set_plain_modulus(const api::Modulus &plain_modulus) override;

    const Scheme &scheme() const override;

    std::size_t poly_modulus_degree() const override;

    const CoeffModulus &coeff_modulus() const override;

    const Modulus &plain_modulus() const override;

    inline const seal::EncryptionParameters &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::EncryptionParameters> underlying_; // ORDER DEPENDENCY
    Scheme scheme_;
    CoeffModulus coeff_modulus_; // ORDER DEPENDENCY
    Modulus plain_modulus_; // ORDER DEPENDENCY
  };
} // namespace seal_backend
} // namespace ufhe
