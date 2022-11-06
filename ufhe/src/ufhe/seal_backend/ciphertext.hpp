#pragma once

#include "seal/seal.h"
#include "ufhe/api/ciphertext.hpp"
#include "ufhe/seal_backend/encryption_context.hpp"
#include <memory>

namespace ufhe
{
namespace seal_backend
{
  class Ciphertext : public api::Ciphertext
  {
    friend class Encryptor;
    friend class Evaluator;

  public:
    Ciphertext() : underlying_(std::make_shared<seal::Ciphertext>()) {}

    Ciphertext(const EncryptionContext &context) : underlying_(std::make_shared<seal::Ciphertext>(context.underlying()))
    {}

    Ciphertext(const Ciphertext &copy) : underlying_(std::make_shared<seal::Ciphertext>(copy.underlying())) {}

    Ciphertext &operator=(const Ciphertext &assign)
    {
      underlying_ = std::make_shared<seal::Ciphertext>(assign.underlying());
      return *this;
    }

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    inline std::size_t coeff_modulus_size() const override { return underlying().coeff_modulus_size(); }

    inline std::size_t poly_modulus_degree() const override { return underlying().poly_modulus_degree(); }

    inline std::size_t size() const override { return underlying().size(); }

    inline bool is_transparent() const override { return underlying().is_transparent(); }

    inline double &scale() const override { return underlying_->scale(); }

    inline const seal::Ciphertext &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::Ciphertext> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
