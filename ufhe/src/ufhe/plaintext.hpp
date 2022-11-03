#pragma once

#include "ufhe/api/iplaintext.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/plaintext.hpp"

namespace ufhe
{
class Plaintext : public api::IPlaintext
{
  friend class Evaluator;

public:
  Plaintext()
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::Plaintext();
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  Plaintext(std::size_t coeff_count)
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::Plaintext(coeff_count);
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  Plaintext(const std::string &hex_poly)
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::Plaintext(hex_poly);
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  Plaintext(const Plaintext &copy) = delete;

  Plaintext &operator=(const Plaintext &assign) = delete;

  ~Plaintext() { delete underlying_; }

  inline api::backend_type backend() const override { return api::backend_type::seal; }

  inline void resize(std::size_t coeff_count) override { underlying().resize(coeff_count); }

  inline void set_zero(std::size_t start_coeff, std::size_t length) override
  {
    underlying().set_zero(start_coeff, length);
  }

  inline std::size_t capacity() const override { return underlying().capacity(); }

  inline std::size_t coeff_count() const override { return underlying().coeff_count(); }

  inline std::string to_string() const override { return underlying().to_string(); }

  inline bool operator==(const api::IPlaintext &compare) const override
  {
    return underlying() == dynamic_cast<const Plaintext &>(compare).underlying();
  }

  inline bool operator!=(const api::IPlaintext &compare) const override
  {
    return underlying() != dynamic_cast<const Plaintext &>(compare).underlying();
  }

private:
  inline api::IPlaintext &underlying() const { return *underlying_; }

  api::IPlaintext *underlying_;
};
} // namespace ufhe
