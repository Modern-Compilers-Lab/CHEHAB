#pragma once

#include "ufhe/api/evaluator.hpp"
#include "ufhe/ciphertext.hpp"
#include "ufhe/encryption_context.hpp"
#include "ufhe/galois_keys.hpp"
#include "ufhe/plaintext.hpp"
#include "ufhe/relin_keys.hpp"
#include "ufhe/seal_backend/evaluator.hpp"
#include <memory>

namespace ufhe
{
class Evaluator : public api::Evaluator
{
public:
  Evaluator(const EncryptionContext &context);

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  inline void negate_inplace(api::Ciphertext &encrypted) const override
  {
    check_strict_compatibility(encrypted);
    underlying().negate_inplace(*static_cast<Ciphertext &>(encrypted).underlying_);
  }

  inline void negate(const api::Ciphertext &encrypted, api::Ciphertext &destination) const override
  {
    check_strict_compatibility(encrypted);
    check_strict_compatibility(destination);
    underlying().negate(
      static_cast<const Ciphertext &>(encrypted).underlying(), *static_cast<Ciphertext &>(destination).underlying_);
  }

  inline void add_inplace(api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2) const override
  {
    check_strict_compatibility(encrypted1);
    check_strict_compatibility(encrypted2);
    underlying().add_inplace(
      *static_cast<Ciphertext &>(encrypted1).underlying_, static_cast<const Ciphertext &>(encrypted2).underlying());
  }

  inline void add(
    const api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2, api::Ciphertext &destination) const override
  {
    check_strict_compatibility(encrypted1);
    check_strict_compatibility(encrypted2);
    check_strict_compatibility(destination);
    underlying().add(
      static_cast<const Ciphertext &>(encrypted1).underlying(),
      static_cast<const Ciphertext &>(encrypted2).underlying(), *static_cast<Ciphertext &>(destination).underlying_);
  }

  inline void sub_inplace(api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2) const override
  {
    check_strict_compatibility(encrypted1);
    check_strict_compatibility(encrypted2);
    underlying().sub_inplace(
      *static_cast<Ciphertext &>(encrypted1).underlying_, static_cast<const Ciphertext &>(encrypted2).underlying());
  }

  inline void sub(
    const api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2, api::Ciphertext &destination) const override
  {
    check_strict_compatibility(encrypted1);
    check_strict_compatibility(encrypted2);
    check_strict_compatibility(destination);
    underlying().sub(
      static_cast<const Ciphertext &>(encrypted1).underlying(),
      static_cast<const Ciphertext &>(encrypted2).underlying(), *static_cast<Ciphertext &>(destination).underlying_);
  }

  inline void multiply_inplace(api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2) const override
  {
    check_strict_compatibility(encrypted1);
    check_strict_compatibility(encrypted2);
    underlying().multiply_inplace(
      *static_cast<Ciphertext &>(encrypted1).underlying_, static_cast<const Ciphertext &>(encrypted2).underlying());
  }

  inline void multiply(
    const api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2, api::Ciphertext &destination) const override
  {
    check_strict_compatibility(encrypted1);
    check_strict_compatibility(encrypted2);
    check_strict_compatibility(destination);
    underlying().multiply(
      static_cast<const Ciphertext &>(encrypted1).underlying(),
      static_cast<const Ciphertext &>(encrypted2).underlying(), *static_cast<Ciphertext &>(destination).underlying_);
  }

  inline void square_inplace(api::Ciphertext &encrypted) const override
  {
    check_strict_compatibility(encrypted);
    underlying().square_inplace(*static_cast<Ciphertext &>(encrypted).underlying_);
  }

  inline void square(const api::Ciphertext &encrypted, api::Ciphertext &destination) const override
  {
    check_strict_compatibility(encrypted);
    check_strict_compatibility(destination);
    underlying().square(
      static_cast<const Ciphertext &>(encrypted).underlying(), *static_cast<Ciphertext &>(destination).underlying_);
  }

  inline void exponentiate_inplace(
    api::Ciphertext &encrypted, std::uint64_t exponent, const api::RelinKeys &relin_keys) const override
  {
    check_strict_compatibility(encrypted);
    check_strict_compatibility(relin_keys);
    underlying().exponentiate_inplace(
      *static_cast<Ciphertext &>(encrypted).underlying_, exponent,
      static_cast<const RelinKeys &>(relin_keys).underlying());
  }

  inline void exponentiate(
    const api::Ciphertext &encrypted, std::uint64_t exponent, const api::RelinKeys &relin_keys,
    api::Ciphertext &destination) const override
  {
    check_strict_compatibility(encrypted);
    check_strict_compatibility(relin_keys);
    check_strict_compatibility(destination);
    underlying().exponentiate(
      static_cast<const Ciphertext &>(encrypted).underlying(), exponent,
      static_cast<const RelinKeys &>(relin_keys).underlying(), *static_cast<Ciphertext &>(destination).underlying_);
  }

  inline void add_plain_inplace(api::Ciphertext &encrypted, const api::Plaintext &plain) const override
  {
    check_strict_compatibility(encrypted);
    check_strict_compatibility(plain);
    underlying().add_plain_inplace(
      *static_cast<Ciphertext &>(encrypted).underlying_, static_cast<const Plaintext &>(plain).underlying());
  }

  inline void add_plain(
    const api::Ciphertext &encrypted, const api::Plaintext &plain, api::Ciphertext &destination) const override
  {
    check_strict_compatibility(encrypted);
    check_strict_compatibility(plain);
    check_strict_compatibility(destination);
    underlying().add_plain(
      static_cast<const Ciphertext &>(encrypted).underlying(), static_cast<const Plaintext &>(plain).underlying(),
      *static_cast<Ciphertext &>(destination).underlying_);
  }

  inline void sub_plain_inplace(api::Ciphertext &encrypted, const api::Plaintext &plain) const override
  {
    check_strict_compatibility(encrypted);
    check_strict_compatibility(plain);
    underlying().sub_plain_inplace(
      *static_cast<Ciphertext &>(encrypted).underlying_, static_cast<const Plaintext &>(plain).underlying());
  }

  inline void sub_plain(
    const api::Ciphertext &encrypted, const api::Plaintext &plain, api::Ciphertext &destination) const override
  {
    check_strict_compatibility(encrypted);
    check_strict_compatibility(plain);
    check_strict_compatibility(destination);
    underlying().sub_plain(
      static_cast<const Ciphertext &>(encrypted).underlying(), static_cast<const Plaintext &>(plain).underlying(),
      *static_cast<Ciphertext &>(destination).underlying_);
  }

  inline void multiply_plain_inplace(api::Ciphertext &encrypted, const api::Plaintext &plain) const override
  {
    check_strict_compatibility(encrypted);
    check_strict_compatibility(plain);
    underlying().multiply_plain_inplace(
      *static_cast<Ciphertext &>(encrypted).underlying_, static_cast<const Plaintext &>(plain).underlying());
  }

  inline void multiply_plain(
    const api::Ciphertext &encrypted, const api::Plaintext &plain, api::Ciphertext &destination) const override
  {
    check_strict_compatibility(encrypted);
    check_strict_compatibility(plain);
    check_strict_compatibility(destination);
    underlying().multiply_plain(
      static_cast<const Ciphertext &>(encrypted).underlying(), static_cast<const Plaintext &>(plain).underlying(),
      *static_cast<Ciphertext &>(destination).underlying_);
  }

  inline void relinearize_inplace(api::Ciphertext &encrypted, const api::RelinKeys &relin_keys) const override
  {
    check_strict_compatibility(encrypted);
    check_strict_compatibility(relin_keys);
    underlying().relinearize_inplace(
      *static_cast<Ciphertext &>(encrypted).underlying_, static_cast<const RelinKeys &>(relin_keys).underlying());
  }

  inline void relinearize(
    const api::Ciphertext &encrypted, const api::RelinKeys &relin_keys, api::Ciphertext &destination) const override
  {
    check_strict_compatibility(encrypted);
    check_strict_compatibility(relin_keys);
    check_strict_compatibility(destination);
    underlying().relinearize(
      static_cast<const Ciphertext &>(encrypted).underlying(), static_cast<const RelinKeys &>(relin_keys).underlying(),
      *static_cast<Ciphertext &>(destination).underlying_);
  }

  inline void mod_switch_to_next_inplace(api::Ciphertext &encrypted) const override
  {
    check_strict_compatibility(encrypted);
    underlying().mod_switch_to_next_inplace(*static_cast<Ciphertext &>(encrypted).underlying_);
  }

  inline void mod_switch_to_next(const api::Ciphertext &encrypted, api::Ciphertext &destination) const override
  {
    check_strict_compatibility(encrypted);
    check_strict_compatibility(destination);
    underlying().mod_switch_to_next(
      static_cast<const Ciphertext &>(encrypted).underlying(), *static_cast<Ciphertext &>(destination).underlying_);
  }

  inline void mod_switch_to_next_inplace(api::Plaintext &plain) const override
  {
    check_strict_compatibility(plain);
    underlying().mod_switch_to_next_inplace(*static_cast<Plaintext &>(plain).underlying_);
  }

  inline void mod_switch_to_next(const api::Plaintext &plain, api::Plaintext &destination) const override
  {
    check_strict_compatibility(plain);
    check_strict_compatibility(destination);
    underlying().mod_switch_to_next(
      static_cast<const Plaintext &>(plain).underlying(), *static_cast<Plaintext &>(destination).underlying_);
  }

  inline void rescale_to_next_inplace(api::Ciphertext &encrypted) const override
  {
    check_strict_compatibility(encrypted);
    underlying().rescale_to_next_inplace(*static_cast<Ciphertext &>(encrypted).underlying_);
  }

  inline void rescale_to_next(const api::Ciphertext &encrypted, api::Ciphertext &destination) const override
  {
    check_strict_compatibility(encrypted);
    check_strict_compatibility(destination);
    underlying().rescale_to_next(
      static_cast<const Ciphertext &>(encrypted).underlying(), *static_cast<Ciphertext &>(destination).underlying_);
  }

  inline void rotate_inplace(api::Ciphertext &encrypted, int steps, const api::GaloisKeys &galois_keys) const override
  {
    check_strict_compatibility(encrypted);
    check_strict_compatibility(galois_keys);
    underlying().rotate_inplace(
      *static_cast<Ciphertext &>(encrypted).underlying_, steps,
      static_cast<const GaloisKeys &>(galois_keys).underlying());
  }

  inline void rotate(
    const api::Ciphertext &encrypted, int steps, const api::GaloisKeys &galois_keys,
    api::Ciphertext &destination) const override
  {
    check_strict_compatibility(encrypted);
    check_strict_compatibility(galois_keys);
    check_strict_compatibility(destination);
    underlying().rotate(
      static_cast<const Ciphertext &>(encrypted).underlying(), steps,
      static_cast<const GaloisKeys &>(galois_keys).underlying(), *static_cast<Ciphertext &>(destination).underlying_);
  }

  inline const api::Evaluator &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::Evaluator> underlying_;
};
} // namespace ufhe
