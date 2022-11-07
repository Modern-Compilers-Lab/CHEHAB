#pragma once

#include "ufhe/api/evaluator.hpp"
#include <memory>

namespace ufhe
{
class EncryptionContext;

class Evaluator : public api::Evaluator
{
public:
  explicit Evaluator(const EncryptionContext &context);

  Evaluator(const Evaluator &copy);

  Evaluator &operator=(const Evaluator &assign);

  Evaluator(Evaluator &&source) = default;

  Evaluator &operator=(Evaluator &&assign) = default;

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  void negate_inplace(api::Ciphertext &encrypted) const override;

  void negate(const api::Ciphertext &encrypted, api::Ciphertext &destination) const override;

  void add_inplace(api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2) const override;

  void add(
    const api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2, api::Ciphertext &destination) const override;

  void sub_inplace(api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2) const override;

  void sub(
    const api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2, api::Ciphertext &destination) const override;

  void multiply_inplace(api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2) const override;

  void multiply(
    const api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2, api::Ciphertext &destination) const override;

  void square_inplace(api::Ciphertext &encrypted) const override;

  void square(const api::Ciphertext &encrypted, api::Ciphertext &destination) const override;

  void exponentiate_inplace(
    api::Ciphertext &encrypted, std::uint64_t exponent, const api::RelinKeys &relin_keys) const override;

  void exponentiate(
    const api::Ciphertext &encrypted, std::uint64_t exponent, const api::RelinKeys &relin_keys,
    api::Ciphertext &destination) const override;

  void add_plain_inplace(api::Ciphertext &encrypted, const api::Plaintext &plain) const override;

  void add_plain(
    const api::Ciphertext &encrypted, const api::Plaintext &plain, api::Ciphertext &destination) const override;

  void sub_plain_inplace(api::Ciphertext &encrypted, const api::Plaintext &plain) const override;

  void sub_plain(
    const api::Ciphertext &encrypted, const api::Plaintext &plain, api::Ciphertext &destination) const override;

  void multiply_plain_inplace(api::Ciphertext &encrypted, const api::Plaintext &plain) const override;

  void multiply_plain(
    const api::Ciphertext &encrypted, const api::Plaintext &plain, api::Ciphertext &destination) const override;

  void relinearize_inplace(api::Ciphertext &encrypted, const api::RelinKeys &relin_keys) const override;

  void relinearize(
    const api::Ciphertext &encrypted, const api::RelinKeys &relin_keys, api::Ciphertext &destination) const override;

  void mod_switch_to_next_inplace(api::Ciphertext &encrypted) const override;

  void mod_switch_to_next(const api::Ciphertext &encrypted, api::Ciphertext &destination) const override;

  void mod_switch_to_next_inplace(api::Plaintext &plain) const override;

  void mod_switch_to_next(const api::Plaintext &plain, api::Plaintext &destination) const override;

  void rescale_to_next_inplace(api::Ciphertext &encrypted) const override;

  void rescale_to_next(const api::Ciphertext &encrypted, api::Ciphertext &destination) const override;

  void rotate_inplace(api::Ciphertext &encrypted, int steps, const api::GaloisKeys &galois_keys) const override;

  void rotate(
    const api::Ciphertext &encrypted, int steps, const api::GaloisKeys &galois_keys,
    api::Ciphertext &destination) const override;

  inline const api::Evaluator &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::Evaluator> underlying_;
};
} // namespace ufhe
