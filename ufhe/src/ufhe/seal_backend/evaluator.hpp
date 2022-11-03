#pragma once

#include "seal/seal.h"
#include "ufhe/api/evaluator.hpp"
#include "ufhe/seal_backend/ciphertext.hpp"
#include "ufhe/seal_backend/encryption_context.hpp"
#include "ufhe/seal_backend/plaintext.hpp"
#include "ufhe/seal_backend/relin_keys.hpp"

namespace ufhe
{
namespace seal_backend
{
  class Evaluator : public api::Evaluator
  {
  public:
    Evaluator(const EncryptionContext &context) : underlying_(seal::Evaluator(context.underlying_)) {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline void negate_inplace(api::Ciphertext &encrypted) const override
    {
      underlying_.negate_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying_);
    }

    inline void negate(const api::Ciphertext &encrypted, api::Ciphertext &destination) const override
    {
      underlying_.negate(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void add_inplace(api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2) const override
    {
      underlying_.add_inplace(
        dynamic_cast<Ciphertext &>(encrypted1).underlying_, dynamic_cast<const Ciphertext &>(encrypted2).underlying_);
    }

    inline void add(
      const api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2, api::Ciphertext &destination) const override
    {
      underlying_.add(
        dynamic_cast<const Ciphertext &>(encrypted1).underlying_,
        dynamic_cast<const Ciphertext &>(encrypted2).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void sub_inplace(api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2) const override
    {
      underlying_.sub_inplace(
        dynamic_cast<Ciphertext &>(encrypted1).underlying_, dynamic_cast<const Ciphertext &>(encrypted2).underlying_);
    }

    inline void sub(
      const api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2, api::Ciphertext &destination) const override
    {
      underlying_.sub(
        dynamic_cast<const Ciphertext &>(encrypted1).underlying_,
        dynamic_cast<const Ciphertext &>(encrypted2).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void multiply_inplace(api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2) const override
    {
      underlying_.multiply_inplace(
        dynamic_cast<Ciphertext &>(encrypted1).underlying_, dynamic_cast<const Ciphertext &>(encrypted2).underlying_);
    }

    inline void multiply(
      const api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2, api::Ciphertext &destination) const override
    {
      underlying_.multiply(
        dynamic_cast<const Ciphertext &>(encrypted1).underlying_,
        dynamic_cast<const Ciphertext &>(encrypted2).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void square_inplace(api::Ciphertext &encrypted) const override
    {
      underlying_.square_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying_);
    }

    inline void square(const api::Ciphertext &encrypted, api::Ciphertext &destination) const override
    {
      underlying_.square(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void exponentiate_inplace(
      api::Ciphertext &encrypted, std::uint64_t exponent, const api::RelinKeys &relin_keys) const override
    {
      underlying_.exponentiate_inplace(
        dynamic_cast<Ciphertext &>(encrypted).underlying_, exponent,
        dynamic_cast<const RelinKeys &>(relin_keys).underlying_);
    }

    inline void exponentiate(
      const api::Ciphertext &encrypted, std::uint64_t exponent, const api::RelinKeys &relin_keys,
      api::Ciphertext &destination) const override
    {
      underlying_.exponentiate(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, exponent,
        dynamic_cast<const RelinKeys &>(relin_keys).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void add_plain_inplace(api::Ciphertext &encrypted, const api::Plaintext &plain) const override
    {
      underlying_.add_plain_inplace(
        dynamic_cast<Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_);
    }

    inline void add_plain(
      const api::Ciphertext &encrypted, const api::Plaintext &plain, api::Ciphertext &destination) const override
    {
      underlying_.add_plain(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_,
        dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void sub_plain_inplace(api::Ciphertext &encrypted, const api::Plaintext &plain) const override
    {
      underlying_.sub_plain_inplace(
        dynamic_cast<Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_);
    }

    inline void sub_plain(
      const api::Ciphertext &encrypted, const api::Plaintext &plain, api::Ciphertext &destination) const override
    {
      underlying_.sub_plain(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_,
        dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void multiply_plain_inplace(api::Ciphertext &encrypted, const api::Plaintext &plain) const override
    {
      underlying_.multiply_plain_inplace(
        dynamic_cast<Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_);
    }

    inline void multiply_plain(
      const api::Ciphertext &encrypted, const api::Plaintext &plain, api::Ciphertext &destination) const override
    {
      underlying_.multiply_plain(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<const Plaintext &>(plain).underlying_,
        dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void relinearize_inplace(api::Ciphertext &encrypted, const api::RelinKeys &relin_keys) const override
    {
      underlying_.relinearize_inplace(
        dynamic_cast<Ciphertext &>(encrypted).underlying_, dynamic_cast<const RelinKeys &>(relin_keys).underlying_);
    }

    inline void relinearize(
      const api::Ciphertext &encrypted, const api::RelinKeys &relin_keys, api::Ciphertext &destination) const override
    {
      underlying_.relinearize(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_,
        dynamic_cast<const RelinKeys &>(relin_keys).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void mod_switch_to_next_inplace(api::Ciphertext &encrypted) const override
    {
      underlying_.mod_switch_to_next_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying_);
    }

    inline void mod_switch_to_next(const api::Ciphertext &encrypted, api::Ciphertext &destination) const override
    {
      underlying_.mod_switch_to_next(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void mod_switch_to_next_inplace(api::Plaintext &plain) const override
    {
      underlying_.mod_switch_to_next_inplace(dynamic_cast<Plaintext &>(plain).underlying_);
    }

    inline void mod_switch_to_next(const api::Plaintext &plain, api::Plaintext &destination) const override
    {
      underlying_.mod_switch_to_next(
        dynamic_cast<const Plaintext &>(plain).underlying_, dynamic_cast<Plaintext &>(destination).underlying_);
    }

    inline void rescale_to_next(const api::Ciphertext &encrypted, api::Ciphertext &destination) const override
    {
      underlying_.rescale_to_next(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

    inline void rescale_to_next_inplace(api::Ciphertext &encrypted) const override
    {
      underlying_.rescale_to_next_inplace(dynamic_cast<Ciphertext &>(encrypted).underlying_);
    }

  private:
    seal::Evaluator underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
