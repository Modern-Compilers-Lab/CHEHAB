#pragma once

#include "ufhe/api/ciphertext.hpp"
#include "ufhe/config.hpp"
#include <memory>

namespace ufhe
{
class EncryptionContext;

class Ciphertext : public api::Ciphertext
{
  friend class Encryptor;
  friend class Evaluator;

public:
  explicit Ciphertext(api::backend_type backend = Config::backend());

  explicit Ciphertext(const EncryptionContext &context);

  Ciphertext(const Ciphertext &copy);

  Ciphertext &operator=(const Ciphertext &assign);

  Ciphertext(Ciphertext &&source) = default;

  Ciphertext &operator=(Ciphertext &&assign) = default;

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  std::size_t coeff_modulus_size() const override;

  std::size_t poly_modulus_degree() const override;

  std::size_t size() const override;

  bool is_transparent() const override;

  double &scale() const override;

  inline const api::Ciphertext &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::Ciphertext> underlying_;
};
} // namespace ufhe
