#pragma once

#include "ufhe/api/evaluator.hpp"
#include "ufhe/ciphertext.hpp"
#include "ufhe/encryption_context.hpp"
#include "ufhe/galois_keys.hpp"
#include "ufhe/plaintext.hpp"
#include "ufhe/relin_keys.hpp"
#include "ufhe/seal_backend/evaluator.hpp"

namespace ufhe
{
class Evaluator : public api::Evaluator
{
public:
  Evaluator(const EncryptionContext &context);

  Evaluator(const Evaluator &copy) = delete;

  Evaluator &operator=(const Evaluator &assign) = delete;

  ~Evaluator() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline void negate_inplace(api::Ciphertext &encrypted) const override
  {
    underlying().negate_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying());
  }

  inline void negate(const api::Ciphertext &encrypted, api::Ciphertext &destination) const override
  {
    underlying().negate(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void add_inplace(api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2) const override
  {
    underlying().add_inplace(
      dynamic_cast<Ciphertext &>(encrypted1).underlying(), dynamic_cast<const Ciphertext &>(encrypted2).underlying());
  }

  inline void add(
    const api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2, api::Ciphertext &destination) const override
  {
    underlying().add(
      dynamic_cast<const Ciphertext &>(encrypted1).underlying(),
      dynamic_cast<const Ciphertext &>(encrypted2).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void sub_inplace(api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2) const override
  {
    underlying().sub_inplace(
      dynamic_cast<Ciphertext &>(encrypted1).underlying(), dynamic_cast<const Ciphertext &>(encrypted2).underlying());
  }

  inline void sub(
    const api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2, api::Ciphertext &destination) const override
  {
    underlying().sub(
      dynamic_cast<const Ciphertext &>(encrypted1).underlying(),
      dynamic_cast<const Ciphertext &>(encrypted2).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void multiply_inplace(api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2) const override
  {
    underlying().multiply_inplace(
      dynamic_cast<Ciphertext &>(encrypted1).underlying(), dynamic_cast<const Ciphertext &>(encrypted2).underlying());
  }

  inline void multiply(
    const api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2, api::Ciphertext &destination) const override
  {
    underlying().multiply(
      dynamic_cast<const Ciphertext &>(encrypted1).underlying(),
      dynamic_cast<const Ciphertext &>(encrypted2).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void square_inplace(api::Ciphertext &encrypted) const override
  {
    underlying().square_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying());
  }

  inline void square(const api::Ciphertext &encrypted, api::Ciphertext &destination) const override
  {
    underlying().square(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void exponentiate_inplace(
    api::Ciphertext &encrypted, std::uint64_t exponent, const api::RelinKeys &relin_keys) const override
  {
    underlying().exponentiate_inplace(
      dynamic_cast<Ciphertext &>(encrypted).underlying(), exponent,
      dynamic_cast<const RelinKeys &>(relin_keys).underlying());
  }

  inline void exponentiate(
    const api::Ciphertext &encrypted, std::uint64_t exponent, const api::RelinKeys &relin_keys,
    api::Ciphertext &destination) const override
  {
    underlying().exponentiate(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), exponent,
      dynamic_cast<const RelinKeys &>(relin_keys).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void add_plain_inplace(api::Ciphertext &encrypted, const api::Plaintext &plain) const override
  {
    underlying().add_plain_inplace(
      dynamic_cast<Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying());
  }

  inline void add_plain(
    const api::Ciphertext &encrypted, const api::Plaintext &plain, api::Ciphertext &destination) const override
  {
    underlying().add_plain(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying(),
      dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void sub_plain_inplace(api::Ciphertext &encrypted, const api::Plaintext &plain) const override
  {
    underlying().sub_plain_inplace(
      dynamic_cast<Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying());
  }

  inline void sub_plain(
    const api::Ciphertext &encrypted, const api::Plaintext &plain, api::Ciphertext &destination) const override
  {
    underlying().sub_plain(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying(),
      dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void multiply_plain_inplace(api::Ciphertext &encrypted, const api::Plaintext &plain) const override
  {
    underlying().multiply_plain_inplace(
      dynamic_cast<Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying());
  }

  inline void multiply_plain(
    const api::Ciphertext &encrypted, const api::Plaintext &plain, api::Ciphertext &destination) const override
  {
    underlying().multiply_plain(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying(),
      dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void relinearize_inplace(api::Ciphertext &encrypted, const api::RelinKeys &relin_keys) const override
  {
    underlying().relinearize_inplace(
      dynamic_cast<Ciphertext &>(encrypted).underlying(), dynamic_cast<const RelinKeys &>(relin_keys).underlying());
  }

  inline void relinearize(
    const api::Ciphertext &encrypted, const api::RelinKeys &relin_keys, api::Ciphertext &destination) const override
  {
    underlying().relinearize(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(),
      dynamic_cast<const RelinKeys &>(relin_keys).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void mod_switch_to_next_inplace(api::Ciphertext &encrypted) const override
  {
    underlying().mod_switch_to_next_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying());
  }

  inline void mod_switch_to_next(const api::Ciphertext &encrypted, api::Ciphertext &destination) const override
  {
    underlying().mod_switch_to_next(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void mod_switch_to_next_inplace(api::Plaintext &plain) const override
  {
    underlying().mod_switch_to_next_inplace(dynamic_cast<Plaintext &>(plain).underlying());
  }

  inline void mod_switch_to_next(const api::Plaintext &plain, api::Plaintext &destination) const override
  {
    underlying().mod_switch_to_next(
      dynamic_cast<const Plaintext &>(plain).underlying(), dynamic_cast<Plaintext &>(destination).underlying());
  }

  inline void rescale_to_next(const api::Ciphertext &encrypted, api::Ciphertext &destination) const override
  {
    underlying().rescale_to_next(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void rescale_to_next_inplace(api::Ciphertext &encrypted) const override
  {
    underlying().rescale_to_next_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying());
  }

  inline void rotate_inplace(api::Ciphertext &encrypted, int steps, const api::GaloisKeys &galois_keys) const override
  {
    underlying().rotate_inplace(
      dynamic_cast<Ciphertext &>(encrypted).underlying(), steps,
      dynamic_cast<const GaloisKeys &>(galois_keys).underlying());
  }

  inline void rotate(
    const api::Ciphertext &encrypted, int steps, const api::GaloisKeys &galois_keys,
    api::Ciphertext &destination) const override
  {
    underlying().rotate(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), steps,
      dynamic_cast<const GaloisKeys &>(galois_keys).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

private:
  inline api::Evaluator &underlying() const { return *underlying_; }

  api::Evaluator *underlying_;
};
} // namespace ufhe
