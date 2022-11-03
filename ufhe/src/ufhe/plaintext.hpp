#pragma once

#include "ufhe/api/plaintext.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/plaintext.hpp"

namespace ufhe
{
class Plaintext : public api::Plaintext
{
  friend class Evaluator;
  friend class BatchEncoder;
  friend class Encryptor;
  friend class Decryptor;

public:
  Plaintext();

  Plaintext(std::size_t coeff_count);

  Plaintext(const std::string &hex_poly);

  Plaintext(const Plaintext &copy) = delete;

  Plaintext &operator=(const Plaintext &assign) = delete;

  ~Plaintext() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline void resize(std::size_t coeff_count) override { underlying().resize(coeff_count); }

  inline void set_zero(std::size_t start_coeff, std::size_t length) override
  {
    underlying().set_zero(start_coeff, length);
  }

  inline std::size_t capacity() const override { return underlying().capacity(); }

  inline std::size_t coeff_count() const override { return underlying().coeff_count(); }

  inline std::string to_string() const override { return underlying().to_string(); }

  inline bool operator==(const api::Plaintext &compare) const override
  {
    return underlying() == dynamic_cast<const Plaintext &>(compare).underlying();
  }

  inline bool operator!=(const api::Plaintext &compare) const override
  {
    return underlying() != dynamic_cast<const Plaintext &>(compare).underlying();
  }

private:
  inline api::Plaintext &underlying() const { return *underlying_; }

  api::Plaintext *underlying_;
};
} // namespace ufhe
