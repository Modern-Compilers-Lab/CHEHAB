#pragma once

#include "seal/seal.h"
#include "ufhe/api/evaluator.hpp"
#include "ufhe/seal_backend/ciphertext.hpp"
#include "ufhe/seal_backend/encryption_context.hpp"
#include "ufhe/seal_backend/galois_keys.hpp"
#include "ufhe/seal_backend/plaintext.hpp"
#include "ufhe/seal_backend/relin_keys.hpp"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <vector>

namespace ufhe
{
namespace seal_backend
{
  class Evaluator : public api::Evaluator
  {
  public:
    Evaluator(const EncryptionContext &context)
      : underlying_(std::make_shared<seal::Evaluator>(context.underlying())), context_(context)
    {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

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
        static_cast<const Ciphertext &>(encrypted).underlying(),
        static_cast<const RelinKeys &>(relin_keys).underlying(), *static_cast<Ciphertext &>(destination).underlying_);
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
      seal::Ciphertext &seal_encrypted = *static_cast<Ciphertext &>(encrypted).underlying_;
      const seal::GaloisKeys &seal_galois_keys = static_cast<const GaloisKeys &>(galois_keys).underlying();
      std::size_t coeff_count = seal_encrypted.poly_modulus_degree();
      std::size_t row_size = coeff_count >> 1;
      if (steps > row_size)
      {
        underlying().rotate_columns_inplace(seal_encrypted, seal_galois_keys);
        return rotate_inplace(encrypted, steps % row_size, galois_keys);
      }
      underlying().rotate_rows_inplace(seal_encrypted, steps, seal_galois_keys);
      seal::Plaintext mask_right;
      seal::Plaintext mask_left;
      gen_symmetric_mask(coeff_count, 0, row_size - steps, mask_left, mask_right);
      seal::Ciphertext right_part;
      underlying().multiply_plain(seal_encrypted, mask_left, right_part);
      underlying().rotate_columns_inplace(right_part, seal_galois_keys);
      underlying().multiply_plain_inplace(seal_encrypted, mask_right);
      underlying().add_inplace(seal_encrypted, right_part);
    }

    inline void rotate(
      const api::Ciphertext &encrypted, int steps, const api::GaloisKeys &galois_keys,
      api::Ciphertext &destination) const override
    {
      check_strict_compatibility(encrypted);
      check_strict_compatibility(galois_keys);
      check_strict_compatibility(destination);
      underlying().rotate_rows(
        static_cast<const Ciphertext &>(encrypted).underlying(), steps,
        static_cast<const GaloisKeys &>(galois_keys).underlying(), *static_cast<Ciphertext &>(destination).underlying_);
    }

    inline const seal::Evaluator &underlying() const { return *underlying_; }

  private:
    void gen_symmetric_mask(
      std::size_t coeff_count, std::size_t start_coeff, std::size_t length, seal::Plaintext &mask,
      seal::Plaintext &mask_inverse) const
    {
      std::size_t row_size = coeff_count >> 1;
      if (length > row_size)
        throw std::invalid_argument("length too large");
      std::vector<std::uint64_t> clear_mask(coeff_count, 1);
      std::vector<std::uint64_t> clear_mask_inverse(coeff_count, 0);
      for (int i = start_coeff; i < length; i++)
      {
        clear_mask[i] = clear_mask[row_size + i] = 0;
        clear_mask_inverse[i] = clear_mask_inverse[row_size + i] = 1;
      }
      seal::BatchEncoder encoder(context_.underlying());
      encoder.encode(clear_mask, mask);
      encoder.encode(clear_mask_inverse, mask_inverse);
    }

    std::shared_ptr<seal::Evaluator> underlying_;
    EncryptionContext context_;
  };
} // namespace seal_backend
} // namespace ufhe
