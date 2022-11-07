#pragma once

#include "ufhe/api/ciphertext.hpp"
#include <memory>

namespace seal
{
class Ciphertext;
} // namespace seal

namespace ufhe
{
namespace seal_backend
{
  class EncryptionContext;

  class Ciphertext : public api::Ciphertext
  {
    friend class Encryptor;
    friend class Evaluator;

  public:
    Ciphertext();

    explicit Ciphertext(const EncryptionContext &context);

    Ciphertext(const Ciphertext &copy);

    Ciphertext &operator=(const Ciphertext &assign);

    Ciphertext(Ciphertext &&source) = default;

    Ciphertext &operator=(Ciphertext &&assign) = default;

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    std::size_t coeff_modulus_size() const override;

    std::size_t poly_modulus_degree() const override;

    std::size_t size() const override;

    bool is_transparent() const override;

    double &scale() const override;

    inline const seal::Ciphertext &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::Ciphertext> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
