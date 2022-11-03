#pragma once

#include "ufhe/api/ievaluator.hpp"
#include "ufhe/ciphertext.hpp"
#include "ufhe/encryptioncontext.hpp"
#include "ufhe/plaintext.hpp"
#include "ufhe/seal_backend/evaluator.hpp"

namespace ufhe
{
class Evaluator : public api::IEvaluator
{
public:
  inline Evaluator(const EncryptionContext &context)
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ =
        new seal_backend::Evaluator(dynamic_cast<const seal_backend::EncryptionContext &>(context.underlying()));
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  Evaluator(const Evaluator &copy) = delete;

  Evaluator &operator=(const Evaluator &assign) = delete;

  ~Evaluator() { delete underlying_; }

  inline api::backend_type backend() const override { return api::backend_type::seal; }

  inline void negate_inplace(api::ICiphertext &encrypted) const override
  {
    underlying().negate_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying());
  }

  inline void negate(const api::ICiphertext &encrypted, api::ICiphertext &destination) const override
  {
    underlying().negate(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void add_inplace(api::ICiphertext &encrypted1, const api::ICiphertext &encrypted2) const override
  {
    underlying().add_inplace(
      dynamic_cast<Ciphertext &>(encrypted1).underlying(), dynamic_cast<const Ciphertext &>(encrypted2).underlying());
  }

  inline void add(const api::ICiphertext &encrypted1, const api::ICiphertext &encrypted2, api::ICiphertext &destination)
    const override
  {
    underlying().add(
      dynamic_cast<const Ciphertext &>(encrypted1).underlying(),
      dynamic_cast<const Ciphertext &>(encrypted2).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void sub_inplace(api::ICiphertext &encrypted1, const api::ICiphertext &encrypted2) const override
  {
    underlying().sub_inplace(
      dynamic_cast<Ciphertext &>(encrypted1).underlying(), dynamic_cast<const Ciphertext &>(encrypted2).underlying());
  }

  inline void sub(const api::ICiphertext &encrypted1, const api::ICiphertext &encrypted2, api::ICiphertext &destination)
    const override
  {
    underlying().sub(
      dynamic_cast<const Ciphertext &>(encrypted1).underlying(),
      dynamic_cast<const Ciphertext &>(encrypted2).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void multiply_inplace(api::ICiphertext &encrypted1, const api::ICiphertext &encrypted2) const override
  {
    underlying().multiply_inplace(
      dynamic_cast<Ciphertext &>(encrypted1).underlying(), dynamic_cast<const Ciphertext &>(encrypted2).underlying());
  }

  inline void multiply(
    const api::ICiphertext &encrypted1, const api::ICiphertext &encrypted2,
    api::ICiphertext &destination) const override
  {
    underlying().multiply(
      dynamic_cast<const Ciphertext &>(encrypted1).underlying(),
      dynamic_cast<const Ciphertext &>(encrypted2).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void square_inplace(api::ICiphertext &encrypted) const override
  {
    underlying().square_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying());
  }

  inline void square(const api::ICiphertext &encrypted, api::ICiphertext &destination) const override
  {
    underlying().square(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void add_plain_inplace(api::ICiphertext &encrypted, const api::IPlaintext &plain) const override
  {
    underlying().add_plain_inplace(
      dynamic_cast<Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying());
  }

  inline void add_plain(
    const api::ICiphertext &encrypted, const api::IPlaintext &plain, api::ICiphertext &destination) const override
  {
    underlying().add_plain(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying(),
      dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void sub_plain_inplace(api::ICiphertext &encrypted, const api::IPlaintext &plain) const override
  {
    underlying().sub_plain_inplace(
      dynamic_cast<Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying());
  }

  inline void sub_plain(
    const api::ICiphertext &encrypted, const api::IPlaintext &plain, api::ICiphertext &destination) const override
  {
    underlying().sub_plain(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying(),
      dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void multiply_plain_inplace(api::ICiphertext &encrypted, const api::IPlaintext &plain) const override
  {
    underlying().multiply_plain_inplace(
      dynamic_cast<Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying());
  }

  inline void multiply_plain(
    const api::ICiphertext &encrypted, const api::IPlaintext &plain, api::ICiphertext &destination) const override
  {
    underlying().multiply_plain(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying(),
      dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void mod_switch_to_next_inplace(api::ICiphertext &encrypted) const override
  {
    underlying().mod_switch_to_next_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying());
  }

  inline void mod_switch_to_next(const api::ICiphertext &encrypted, api::ICiphertext &destination) const override
  {
    underlying().mod_switch_to_next(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void mod_switch_to_next_inplace(api::IPlaintext &plain) const override
  {
    underlying().mod_switch_to_next_inplace(dynamic_cast<Plaintext &>(plain).underlying());
  }

  inline void mod_switch_to_next(const api::IPlaintext &plain, api::IPlaintext &destination) const override
  {
    underlying().mod_switch_to_next(
      dynamic_cast<const Plaintext &>(plain).underlying(), dynamic_cast<Plaintext &>(destination).underlying());
  }

  inline void rescale_to_next(const api::ICiphertext &encrypted, api::ICiphertext &destination) const override
  {
    underlying().rescale_to_next(
      dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
  }

  inline void rescale_to_next_inplace(api::ICiphertext &encrypted) const override
  {
    underlying().rescale_to_next_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying());
  }

private:
  inline api::IEvaluator &underlying() const { return *underlying_; }

  api::IEvaluator *underlying_;
};
} // namespace ufhe
