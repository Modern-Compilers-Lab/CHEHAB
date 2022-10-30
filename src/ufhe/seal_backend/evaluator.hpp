#pragma once

#include "seal/seal.h"
#include "ufhe/api/ievaluator.hpp"
#include "ufhe/seal_backend/ciphertext.hpp"
#include "ufhe/seal_backend/encryptioncontext.hpp"
#include "ufhe/seal_backend/plaintext.hpp"

namespace ufhe
{
namespace seal_backend
{
  class Evaluator : public api::IEvaluator
  {
  public:
    inline Evaluator(const EncryptionContext &context) : underlying_(seal::Evaluator(context.underlying_)) {}

    inline api::BackendType backend() const override { return api::BackendType::seal; }

    inline void negate_inplace(api::ICiphertext &encrypted) const override
    {
      underlying_.negate_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying_);
    }

    inline void negate(const api::ICiphertext &encrypted, api::ICiphertext &destination) const override
    {
      underlying_.negate(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void add_inplace(api::ICiphertext &encrypted1, const api::ICiphertext &encrypted2) const override
    {
      underlying_.add_inplace(
        dynamic_cast<Ciphertext &>(encrypted1).underlying_, dynamic_cast<const Ciphertext &>(encrypted2).underlying_);
    }

    inline void add(
      const api::ICiphertext &encrypted1, const api::ICiphertext &encrypted2,
      api::ICiphertext &destination) const override
    {
      underlying_.add(
        dynamic_cast<const Ciphertext &>(encrypted1).underlying_,
        dynamic_cast<const Ciphertext &>(encrypted2).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void sub_inplace(api::ICiphertext &encrypted1, const api::ICiphertext &encrypted2) const override
    {
      underlying_.sub_inplace(
        dynamic_cast<Ciphertext &>(encrypted1).underlying_, dynamic_cast<const Ciphertext &>(encrypted2).underlying_);
    }

    inline void sub(
      const api::ICiphertext &encrypted1, const api::ICiphertext &encrypted2,
      api::ICiphertext &destination) const override
    {
      underlying_.sub(
        dynamic_cast<const Ciphertext &>(encrypted1).underlying_,
        dynamic_cast<const Ciphertext &>(encrypted2).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void multiply_inplace(api::ICiphertext &encrypted1, const api::ICiphertext &encrypted2) const override
    {
      underlying_.multiply_inplace(
        dynamic_cast<Ciphertext &>(encrypted1).underlying_, dynamic_cast<const Ciphertext &>(encrypted2).underlying_);
    }

    inline void multiply(
      const api::ICiphertext &encrypted1, const api::ICiphertext &encrypted2,
      api::ICiphertext &destination) const override
    {
      underlying_.multiply(
        dynamic_cast<const Ciphertext &>(encrypted1).underlying_,
        dynamic_cast<const Ciphertext &>(encrypted2).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void square_inplace(api::ICiphertext &encrypted) const override
    {
      underlying_.square_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying_);
    }

    inline void square(const api::ICiphertext &encrypted, api::ICiphertext &destination) const override
    {
      underlying_.square(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void add_plain_inplace(api::ICiphertext &encrypted, const api::IPlaintext &plain) const override
    {
      underlying_.add_plain_inplace(
        dynamic_cast<Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_);
    }

    inline void add_plain(
      const api::ICiphertext &encrypted, const api::IPlaintext &plain, api::ICiphertext &destination) const override
    {
      underlying_.add_plain(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_,
        dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void sub_plain_inplace(api::ICiphertext &encrypted, const api::IPlaintext &plain) const override
    {
      underlying_.sub_plain_inplace(
        dynamic_cast<Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_);
    }

    inline void sub_plain(
      const api::ICiphertext &encrypted, const api::IPlaintext &plain, api::ICiphertext &destination) const override
    {
      underlying_.sub_plain(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_,
        dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void multiply_plain_inplace(api::ICiphertext &encrypted, const api::IPlaintext &plain) const override
    {
      underlying_.multiply_plain_inplace(
        dynamic_cast<Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_);
    }

    inline void multiply_plain(
      const api::ICiphertext &encrypted, const api::IPlaintext &plain, api::ICiphertext &destination) const override
    {
      underlying_.multiply_plain(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_,
        dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void mod_switch_to_next_inplace(api::ICiphertext &encrypted) const override
    {
      underlying_.mod_switch_to_next_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying_);
    }

    inline void mod_switch_to_next(const api::ICiphertext &encrypted, api::ICiphertext &destination) const override
    {
      underlying_.mod_switch_to_next(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void mod_switch_to_next_inplace(api::IPlaintext &plain) const override
    {
      underlying_.mod_switch_to_next_inplace(dynamic_cast<Plaintext &>(plain).underlying_);
    }

    inline void mod_switch_to_next(const api::IPlaintext &plain, api::IPlaintext &destination) const override
    {
      underlying_.mod_switch_to_next(
        dynamic_cast<const Plaintext &>(plain).underlying_, dynamic_cast<Plaintext &>(destination).underlying_);
    }

    inline void rescale_to_next(const api::ICiphertext &encrypted, api::ICiphertext &destination) const override
    {
      underlying_.rescale_to_next(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void rescale_to_next_inplace(api::ICiphertext &encrypted) const override
    {
      underlying_.rescale_to_next_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying_);
    }

  private:
    seal::Evaluator underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
