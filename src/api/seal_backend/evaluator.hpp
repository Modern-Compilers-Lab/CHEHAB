#pragma once

#include "../ievaluator.hpp"
#include "ciphertext.hpp"
#include "encryptioncontext.hpp"
#include "implementation.hpp"
#include "plaintext.hpp"
#include "seal/seal.h"

namespace ufhe
{
namespace seal_backend
{
  class Evaluator : public Implementation, public IEvaluator
  {
  public:
    inline Evaluator(const EncryptionContext &context) : Evaluator(new seal::Evaluator(context.underlying()), true) {}

    inline Evaluator(const Evaluator &copy) : Evaluator(copy.underlying_, false) {}

    Evaluator &operator=(const Evaluator &assign) = delete;

    inline ~Evaluator()
    {
      if (is_owner_)
        delete underlying_;
    }

    inline void negate_inplace(ICiphertext &encrypted) const override
    {
      underlying().negate_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying());
    }

    inline void negate(const ICiphertext &encrypted, ICiphertext &destination) const override
    {
      underlying().negate(
        dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
    }

    inline void add_inplace(ICiphertext &encrypted1, const ICiphertext &encrypted2) const override
    {
      underlying().add_inplace(
        dynamic_cast<Ciphertext &>(encrypted1).underlying(), dynamic_cast<const Ciphertext &>(encrypted2).underlying());
    }

    inline void add(
      const ICiphertext &encrypted1, const ICiphertext &encrypted2, ICiphertext &destination) const override
    {
      underlying().add(
        dynamic_cast<const Ciphertext &>(encrypted1).underlying(),
        dynamic_cast<const Ciphertext &>(encrypted2).underlying(),
        dynamic_cast<Ciphertext &>(destination).underlying());
    }

    inline void sub_inplace(ICiphertext &encrypted1, const ICiphertext &encrypted2) const override
    {
      underlying().sub_inplace(
        dynamic_cast<Ciphertext &>(encrypted1).underlying(), dynamic_cast<const Ciphertext &>(encrypted2).underlying());
    }

    inline void sub(
      const ICiphertext &encrypted1, const ICiphertext &encrypted2, ICiphertext &destination) const override
    {
      underlying().sub(
        dynamic_cast<const Ciphertext &>(encrypted1).underlying(),
        dynamic_cast<const Ciphertext &>(encrypted2).underlying(),
        dynamic_cast<Ciphertext &>(destination).underlying());
    }

    inline void multiply_inplace(ICiphertext &encrypted1, const ICiphertext &encrypted2) const override
    {
      underlying().multiply_inplace(
        dynamic_cast<Ciphertext &>(encrypted1).underlying(), dynamic_cast<const Ciphertext &>(encrypted2).underlying());
    }

    inline void multiply(
      const ICiphertext &encrypted1, const ICiphertext &encrypted2, ICiphertext &destination) const override
    {
      underlying().multiply(
        dynamic_cast<const Ciphertext &>(encrypted1).underlying(),
        dynamic_cast<const Ciphertext &>(encrypted2).underlying(),
        dynamic_cast<Ciphertext &>(destination).underlying());
    }

    inline void square_inplace(ICiphertext &encrypted) const override
    {
      underlying().square_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying());
    }

    inline void square(const ICiphertext &encrypted, ICiphertext &destination) const override
    {
      underlying().square(
        dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
    }

    inline void add_plain_inplace(ICiphertext &encrypted, const IPlaintext &plain) const override
    {
      underlying().add_plain_inplace(
        dynamic_cast<Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying());
    }

    inline void add_plain(
      const ICiphertext &encrypted, const IPlaintext &plain, ICiphertext &destination) const override
    {
      underlying().add_plain(
        dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying(),
        dynamic_cast<Ciphertext &>(destination).underlying());
    }

    inline void sub_plain_inplace(ICiphertext &encrypted, const IPlaintext &plain) const override
    {
      underlying().sub_plain_inplace(
        dynamic_cast<Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying());
    }

    inline void sub_plain(
      const ICiphertext &encrypted, const IPlaintext &plain, ICiphertext &destination) const override
    {
      underlying().sub_plain(
        dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying(),
        dynamic_cast<Ciphertext &>(destination).underlying());
    }

    inline void multiply_plain_inplace(ICiphertext &encrypted, const IPlaintext &plain) const override
    {
      underlying().multiply_plain_inplace(
        dynamic_cast<Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying());
    }

    inline void multiply_plain(
      const ICiphertext &encrypted, const IPlaintext &plain, ICiphertext &destination) const override
    {
      underlying().multiply_plain(
        dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<const Plaintext &>(plain).underlying(),
        dynamic_cast<Ciphertext &>(destination).underlying());
    }

    inline void mod_switch_to_next_inplace(ICiphertext &encrypted) const override
    {
      underlying().mod_switch_to_next_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying());
    }

    inline void mod_switch_to_next(const ICiphertext &encrypted, ICiphertext &destination) const override
    {
      underlying().mod_switch_to_next(
        dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
    }

    inline void mod_switch_to_next_inplace(IPlaintext &plain) const override
    {
      underlying().mod_switch_to_next_inplace(dynamic_cast<Plaintext &>(plain).underlying());
    }

    inline void mod_switch_to_next(const IPlaintext &plain, IPlaintext &destination) const override
    {
      underlying().mod_switch_to_next(
        dynamic_cast<const Plaintext &>(plain).underlying(), dynamic_cast<Plaintext &>(destination).underlying());
    }

    inline void rescale_to_next(const ICiphertext &encrypted, ICiphertext &destination) const override
    {
      underlying().rescale_to_next(
        dynamic_cast<const Ciphertext &>(encrypted).underlying(), dynamic_cast<Ciphertext &>(destination).underlying());
    }

    inline void rescale_to_next_inplace(ICiphertext &encrypted) const override
    {
      underlying().rescale_to_next_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying());
    }

  private:
    inline Evaluator(seal::Evaluator *seal_skey, bool is_owner) : underlying_(seal_skey), is_owner_(is_owner) {}

    inline seal::Evaluator &underlying() const { return *underlying_; }

    seal::Evaluator *underlying_;
    bool is_owner_;
  };
} // namespace seal_backend
} // namespace ufhe
