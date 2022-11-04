#pragma once

#include "seal/seal.h"
#include "ufhe/api/evaluator.hpp"
#include "ufhe/seal_backend/ciphertext.hpp"
#include "ufhe/seal_backend/encryption_context.hpp"
#include "ufhe/seal_backend/galois_keys.hpp"
#include "ufhe/seal_backend/plaintext.hpp"
#include "ufhe/seal_backend/relin_keys.hpp"
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace ufhe
{
namespace seal_backend
{
  class Evaluator : public api::Evaluator
  {
  public:
    Evaluator(const EncryptionContext &context) : underlying_(seal::Evaluator(context.underlying_)), context_(context)
    {}

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

    inline void rotate_inplace(api::Ciphertext &encrypted, int steps, const api::GaloisKeys &galois_keys) const override
    {
      seal::Ciphertext &seal_encrypted = dynamic_cast<Ciphertext &>(encrypted).underlying_;
      const seal::GaloisKeys &seal_galois_keys = dynamic_cast<const GaloisKeys &>(galois_keys).underlying_;
      std::size_t coeff_count = seal_encrypted.poly_modulus_degree();
      std::size_t row_size = coeff_count >> 1;
      if (steps > row_size)
      {
        underlying_.rotate_columns_inplace(seal_encrypted, seal_galois_keys);
        return rotate_inplace(encrypted, steps % row_size, galois_keys);
      }
      underlying_.rotate_rows_inplace(seal_encrypted, steps, seal_galois_keys);
      seal::Plaintext mask_right;
      seal::Plaintext mask_left;
      gen_symmetric_mask(coeff_count, 0, row_size - steps, mask_left, mask_right);
      seal::Ciphertext right_part;
      underlying_.multiply_plain(seal_encrypted, mask_left, right_part);
      underlying_.rotate_columns_inplace(right_part, seal_galois_keys);
      underlying_.multiply_plain_inplace(seal_encrypted, mask_right);
      underlying_.add_inplace(seal_encrypted, right_part);
    }

    inline void rotate(
      const api::Ciphertext &encrypted, int steps, const api::GaloisKeys &galois_keys,
      api::Ciphertext &destination) const override
    {
      underlying_.rotate_rows(
        dynamic_cast<const Ciphertext &>(encrypted).underlying_, steps,
        dynamic_cast<const GaloisKeys &>(galois_keys).underlying_, dynamic_cast<Ciphertext &>(destination).underlying_);
    }

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
      seal::BatchEncoder encoder(context_.underlying_);
      encoder.encode(clear_mask, mask);
      encoder.encode(clear_mask_inverse, mask_inverse);
    }

    seal::Evaluator underlying_;
    EncryptionContext context_;
  };
} // namespace seal_backend
} // namespace ufhe
