#pragma once

#include "ufhe/api/iciphertext.hpp"
#include "ufhe/config.hpp"
#include "ufhe/encryptioncontext.hpp"
#include "ufhe/seal_backend/ciphertext.hpp"

namespace ufhe
{
class Ciphertext : public api::ICiphertext
{
  friend class Evaluator;

public:
  Ciphertext()
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::Ciphertext();
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  Ciphertext(const EncryptionContext &context)
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ =
        new seal_backend::Ciphertext(dynamic_cast<const seal_backend::EncryptionContext &>(context.underlying()));
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  Ciphertext(const Ciphertext &copy) = delete;

  Ciphertext &operator=(const Ciphertext &assign) = delete;

  ~Ciphertext() { delete underlying_; }

  inline api::backend_type backend() const override { return api::backend_type::seal; }

  inline std::size_t coeff_modulus_size() const override { return underlying().coeff_modulus_size(); }

  inline std::size_t poly_modulus_degree() const override { return underlying().poly_modulus_degree(); }

  inline std::size_t size() const override { return underlying().size(); }

  inline bool is_transparent() const override { return underlying().is_transparent(); }

  inline double &scale() override { return underlying().scale(); }

private:
  inline api::ICiphertext &underlying() const { return *underlying_; }

  api::ICiphertext *underlying_;
};
} // namespace ufhe
