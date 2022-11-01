#pragma once

#include "seal/seal.h"
#include "ufhe/api/iplaintext.hpp"

namespace ufhe
{
namespace seal_backend
{
  class Plaintext : public api::IPlaintext
  {
    friend class BatchEncoder;
    friend class Encryptor;
    friend class Decryptor;
    friend class Evaluator;

  public:
    inline Plaintext() : underlying_(seal::Plaintext()) {}

    inline Plaintext(std::size_t coeff_count) : underlying_(seal::Plaintext(coeff_count)) {}

    inline Plaintext(const std::string &hex_poly) : underlying_(seal::Plaintext(hex_poly)) {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline void resize(std::size_t coeff_count) override { underlying_.resize(coeff_count); }

    inline void set_zero(std::size_t start_coeff, std::size_t length) override
    {
      underlying_.set_zero(start_coeff, length);
    }

    inline std::size_t capacity() const override { return underlying_.capacity(); }

    inline std::size_t coeff_count() const override { return underlying_.coeff_count(); }

    inline std::string to_string() const override { return underlying_.to_string(); }

    inline bool operator==(const api::IPlaintext &compare) const override
    {
      return underlying_ == dynamic_cast<const Plaintext &>(compare).underlying_;
    }

    inline bool operator!=(const api::IPlaintext &compare) const override
    {
      return underlying_ != dynamic_cast<const Plaintext &>(compare).underlying_;
    }

  private:
    seal::Plaintext underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
