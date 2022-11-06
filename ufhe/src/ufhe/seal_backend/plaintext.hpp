#pragma once

#include "seal/seal.h"
#include "ufhe/api/plaintext.hpp"
#include <memory>

namespace ufhe
{
namespace seal_backend
{
  class Plaintext : public api::Plaintext
  {
    friend class BatchEncoder;
    friend class Decryptor;
    friend class Evaluator;

  public:
    Plaintext() : underlying_(std::make_shared<seal::Plaintext>()) {}

    Plaintext(std::size_t coeff_count) : underlying_(std::make_shared<seal::Plaintext>(coeff_count)) {}

    Plaintext(const std::string &hex_poly) : underlying_(std::make_shared<seal::Plaintext>(hex_poly)) {}

    Plaintext(const Plaintext &copy) : underlying_(std::make_shared<seal::Plaintext>(copy.underlying())) {}

    Plaintext &operator=(const Plaintext &assign)
    {
      underlying_ = std::make_shared<seal::Plaintext>(assign.underlying());
      return *this;
    }

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    inline void resize(std::size_t coeff_count) override { underlying_->resize(coeff_count); }

    inline void set_zero(std::size_t start_coeff, std::size_t length) override
    {
      underlying_->set_zero(start_coeff, length);
    }

    inline std::size_t capacity() const override { return underlying().capacity(); }

    inline std::size_t coeff_count() const override { return underlying().coeff_count(); }

    inline std::string to_string() const override { return underlying().to_string(); }

    inline bool operator==(const api::Plaintext &compare) const override
    {
      check_strict_compatibility(compare);
      return underlying() == static_cast<const Plaintext &>(compare).underlying();
    }

    inline bool operator!=(const api::Plaintext &compare) const override
    {
      check_strict_compatibility(compare);
      return underlying() != static_cast<const Plaintext &>(compare).underlying();
    }

    inline const seal::Plaintext &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::Plaintext> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
