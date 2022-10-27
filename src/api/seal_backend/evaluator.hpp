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
    inline Evaluator(const EncryptionContext &context) : underlying_(seal::Evaluator(context.underlying_)) {}

    inline void negate_inplace(ICiphertext &encrypted) const override
    {
      underlying_.negate_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying_);
    }

    inline void negate(const ICiphertext &encrypted, ICiphertext &destination) const override
    {
      underlying_.negate(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void add_inplace(ICiphertext &encrypted1, const ICiphertext &encrypted2) const override
    {
      underlying_.add_inplace(
        dynamic_cast<Ciphertext &>(encrypted1).underlying_, dynamic_cast<const Ciphertext &>(encrypted2).underlying_);
    }

    inline void add(
      const ICiphertext &encrypted1, const ICiphertext &encrypted2, ICiphertext &destination) const override
    {
      underlying_.add(
        dynamic_cast<const Ciphertext &>(encrypted1).underlying_,
        dynamic_cast<const Ciphertext &>(encrypted2).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void sub_inplace(ICiphertext &encrypted1, const ICiphertext &encrypted2) const override
    {
      underlying_.sub_inplace(
        dynamic_cast<Ciphertext &>(encrypted1).underlying_, dynamic_cast<const Ciphertext &>(encrypted2).underlying_);
    }

    inline void sub(
      const ICiphertext &encrypted1, const ICiphertext &encrypted2, ICiphertext &destination) const override
    {
      underlying_.sub(
        dynamic_cast<const Ciphertext &>(encrypted1).underlying_,
        dynamic_cast<const Ciphertext &>(encrypted2).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void multiply_inplace(ICiphertext &encrypted1, const ICiphertext &encrypted2) const override
    {
      underlying_.multiply_inplace(
        dynamic_cast<Ciphertext &>(encrypted1).underlying_, dynamic_cast<const Ciphertext &>(encrypted2).underlying_);
    }

    inline void multiply(
      const ICiphertext &encrypted1, const ICiphertext &encrypted2, ICiphertext &destination) const override
    {
      underlying_.multiply(
        dynamic_cast<const Ciphertext &>(encrypted1).underlying_,
        dynamic_cast<const Ciphertext &>(encrypted2).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void square_inplace(ICiphertext &encrypted) const override
    {
      underlying_.square_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying_);
    }

    inline void square(const ICiphertext &encrypted, ICiphertext &destination) const override
    {
      underlying_.square(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void add_plain_inplace(ICiphertext &encrypted, const IPlaintext &plain) const override
    {
      underlying_.add_plain_inplace(
        dynamic_cast<Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_);
    }

    inline void add_plain(
      const ICiphertext &encrypted, const IPlaintext &plain, ICiphertext &destination) const override
    {
      underlying_.add_plain(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_,
        dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void sub_plain_inplace(ICiphertext &encrypted, const IPlaintext &plain) const override
    {
      underlying_.sub_plain_inplace(
        dynamic_cast<Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_);
    }

    inline void sub_plain(
      const ICiphertext &encrypted, const IPlaintext &plain, ICiphertext &destination) const override
    {
      underlying_.sub_plain(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_,
        dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void multiply_plain_inplace(ICiphertext &encrypted, const IPlaintext &plain) const override
    {
      underlying_.multiply_plain_inplace(
        dynamic_cast<Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_);
    }

    inline void multiply_plain(
      const ICiphertext &encrypted, const IPlaintext &plain, ICiphertext &destination) const override
    {
      underlying_.multiply_plain(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_,
        dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void mod_switch_to_next_inplace(ICiphertext &encrypted) const override
    {
      underlying_.mod_switch_to_next_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying_);
    }

    inline void mod_switch_to_next(const ICiphertext &encrypted, ICiphertext &destination) const override
    {
      underlying_.mod_switch_to_next(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void mod_switch_to_next_inplace(IPlaintext &plain) const override
    {
      underlying_.mod_switch_to_next_inplace(dynamic_cast<Plaintext &>(plain).underlying_);
    }

    inline void mod_switch_to_next(const IPlaintext &plain, IPlaintext &destination) const override
    {
      underlying_.mod_switch_to_next(
        dynamic_cast<const Plaintext &>(plain).underlying_, dynamic_cast<Plaintext &>(destination).underlying_);
    }

    inline void rescale_to_next(const ICiphertext &encrypted, ICiphertext &destination) const override
    {
      underlying_.rescale_to_next(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void rescale_to_next_inplace(ICiphertext &encrypted) const override
    {
      underlying_.rescale_to_next_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying_);
    }

  private:
    seal::Evaluator underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
