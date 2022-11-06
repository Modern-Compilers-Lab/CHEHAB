#pragma once

#include "ufhe/api/ciphertext.hpp"
#include "ufhe/config.hpp"
#include "ufhe/encryption_context.hpp"
#include "ufhe/seal_backend/ciphertext.hpp"
#include <memory>

namespace ufhe
{
class Ciphertext : public api::Ciphertext
{
  friend class Encryptor;
  friend class Evaluator;

public:
  Ciphertext();

  Ciphertext(const EncryptionContext &context);

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline std::size_t coeff_modulus_size() const override { return underlying().coeff_modulus_size(); }

  inline std::size_t poly_modulus_degree() const override { return underlying().poly_modulus_degree(); }

  inline std::size_t size() const override { return underlying().size(); }

  inline bool is_transparent() const override { return underlying().is_transparent(); }

  inline double &scale() const override { return underlying().scale(); }

  inline const api::Ciphertext &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::Ciphertext> underlying_;
};
} // namespace ufhe
