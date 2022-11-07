#pragma once

#include "ufhe/api/plaintext.hpp"
#include "ufhe/config.hpp"
#include <memory>

namespace ufhe
{
class Plaintext : public api::Plaintext
{
  friend class BatchEncoder;
  friend class Decryptor;
  friend class Evaluator;

public:
  explicit Plaintext(api::backend_type backend = Config::backend());

  Plaintext(api::backend_type backend, std::size_t coeff_count);

  Plaintext(std::size_t coeff_count);

  Plaintext(api::backend_type backend, const std::string &hex_poly);

  Plaintext(const std::string &hex_poly);

  Plaintext(const Plaintext &copy);

  Plaintext &operator=(const Plaintext &assign);

  Plaintext(Plaintext &&source) = default;

  Plaintext &operator=(Plaintext &&assign) = default;

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  void resize(std::size_t coeff_count) override;

  void set_zero(std::size_t start_coeff, std::size_t length) override;

  std::size_t capacity() const override;

  std::size_t coeff_count() const override;

  std::string to_string() const override;

  bool operator==(const api::Plaintext &compare) const override;

  bool operator!=(const api::Plaintext &compare) const override;

  inline const api::Plaintext &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::Plaintext> underlying_;
};
} // namespace ufhe
