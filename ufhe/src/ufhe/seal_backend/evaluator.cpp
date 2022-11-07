#include "ufhe/seal_backend/evaluator.hpp"
#include "seal/batchencoder.h"
#include "seal/evaluator.h"
#include "ufhe/seal_backend/ciphertext.hpp"
#include "ufhe/seal_backend/galois_keys.hpp"
#include "ufhe/seal_backend/plaintext.hpp"
#include "ufhe/seal_backend/relin_keys.hpp"

namespace ufhe
{
namespace seal_backend
{
  Evaluator::Evaluator(const EncryptionContext &context)
    : underlying_(std::make_shared<seal::Evaluator>(context.underlying())), context_(context)
  {}

  void Evaluator::negate_inplace(api::Ciphertext &encrypted) const
  {
    underlying().negate_inplace(*safe_static_cast<Ciphertext &>(encrypted).underlying_);
  }

  void Evaluator::negate(const api::Ciphertext &encrypted, api::Ciphertext &destination) const
  {
    underlying().negate(
      safe_static_cast<const Ciphertext &>(encrypted).underlying(),
      *safe_static_cast<Ciphertext &>(destination).underlying_);
  }

  void Evaluator::add_inplace(api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2) const
  {
    underlying().add_inplace(
      *safe_static_cast<Ciphertext &>(encrypted1).underlying_,
      safe_static_cast<const Ciphertext &>(encrypted2).underlying());
  }

  void Evaluator::add(
    const api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2, api::Ciphertext &destination) const
  {
    underlying().add(
      safe_static_cast<const Ciphertext &>(encrypted1).underlying(),
      safe_static_cast<const Ciphertext &>(encrypted2).underlying(),
      *safe_static_cast<Ciphertext &>(destination).underlying_);
  }

  void Evaluator::sub_inplace(api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2) const
  {
    underlying().sub_inplace(
      *safe_static_cast<Ciphertext &>(encrypted1).underlying_,
      safe_static_cast<const Ciphertext &>(encrypted2).underlying());
  }

  void Evaluator::sub(
    const api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2, api::Ciphertext &destination) const
  {
    underlying().sub(
      safe_static_cast<const Ciphertext &>(encrypted1).underlying(),
      safe_static_cast<const Ciphertext &>(encrypted2).underlying(),
      *safe_static_cast<Ciphertext &>(destination).underlying_);
  }

  void Evaluator::multiply_inplace(api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2) const
  {
    underlying().multiply_inplace(
      *safe_static_cast<Ciphertext &>(encrypted1).underlying_,
      safe_static_cast<const Ciphertext &>(encrypted2).underlying());
  }

  void Evaluator::multiply(
    const api::Ciphertext &encrypted1, const api::Ciphertext &encrypted2, api::Ciphertext &destination) const
  {
    underlying().multiply(
      safe_static_cast<const Ciphertext &>(encrypted1).underlying(),
      safe_static_cast<const Ciphertext &>(encrypted2).underlying(),
      *safe_static_cast<Ciphertext &>(destination).underlying_);
  }

  void Evaluator::square_inplace(api::Ciphertext &encrypted) const
  {
    underlying().square_inplace(*safe_static_cast<Ciphertext &>(encrypted).underlying_);
  }

  void Evaluator::square(const api::Ciphertext &encrypted, api::Ciphertext &destination) const
  {
    underlying().square(
      safe_static_cast<const Ciphertext &>(encrypted).underlying(),
      *safe_static_cast<Ciphertext &>(destination).underlying_);
  }

  void Evaluator::exponentiate_inplace(
    api::Ciphertext &encrypted, std::uint64_t exponent, const api::RelinKeys &relin_keys) const
  {
    underlying().exponentiate_inplace(
      *safe_static_cast<Ciphertext &>(encrypted).underlying_, exponent,
      safe_static_cast<const RelinKeys &>(relin_keys).underlying());
  }

  void Evaluator::exponentiate(
    const api::Ciphertext &encrypted, std::uint64_t exponent, const api::RelinKeys &relin_keys,
    api::Ciphertext &destination) const
  {
    underlying().exponentiate(
      safe_static_cast<const Ciphertext &>(encrypted).underlying(), exponent,
      safe_static_cast<const RelinKeys &>(relin_keys).underlying(),
      *safe_static_cast<Ciphertext &>(destination).underlying_);
  }

  void Evaluator::add_plain_inplace(api::Ciphertext &encrypted, const api::Plaintext &plain) const
  {
    underlying().add_plain_inplace(
      *safe_static_cast<Ciphertext &>(encrypted).underlying_, safe_static_cast<const Plaintext &>(plain).underlying());
  }

  void Evaluator::add_plain(
    const api::Ciphertext &encrypted, const api::Plaintext &plain, api::Ciphertext &destination) const
  {
    underlying().add_plain(
      safe_static_cast<const Ciphertext &>(encrypted).underlying(),
      safe_static_cast<const Plaintext &>(plain).underlying(),
      *safe_static_cast<Ciphertext &>(destination).underlying_);
  }

  void Evaluator::sub_plain_inplace(api::Ciphertext &encrypted, const api::Plaintext &plain) const
  {
    underlying().sub_plain_inplace(
      *safe_static_cast<Ciphertext &>(encrypted).underlying_, safe_static_cast<const Plaintext &>(plain).underlying());
  }

  void Evaluator::sub_plain(
    const api::Ciphertext &encrypted, const api::Plaintext &plain, api::Ciphertext &destination) const
  {
    underlying().sub_plain(
      safe_static_cast<const Ciphertext &>(encrypted).underlying(),
      safe_static_cast<const Plaintext &>(plain).underlying(),
      *safe_static_cast<Ciphertext &>(destination).underlying_);
  }

  void Evaluator::multiply_plain_inplace(api::Ciphertext &encrypted, const api::Plaintext &plain) const
  {
    underlying().multiply_plain_inplace(
      *safe_static_cast<Ciphertext &>(encrypted).underlying_, safe_static_cast<const Plaintext &>(plain).underlying());
  }

  void Evaluator::multiply_plain(
    const api::Ciphertext &encrypted, const api::Plaintext &plain, api::Ciphertext &destination) const
  {
    underlying().multiply_plain(
      safe_static_cast<const Ciphertext &>(encrypted).underlying(),
      safe_static_cast<const Plaintext &>(plain).underlying(),
      *safe_static_cast<Ciphertext &>(destination).underlying_);
  }

  void Evaluator::relinearize_inplace(api::Ciphertext &encrypted, const api::RelinKeys &relin_keys) const
  {
    underlying().relinearize_inplace(
      *safe_static_cast<Ciphertext &>(encrypted).underlying_,
      safe_static_cast<const RelinKeys &>(relin_keys).underlying());
  }

  void Evaluator::relinearize(
    const api::Ciphertext &encrypted, const api::RelinKeys &relin_keys, api::Ciphertext &destination) const
  {
    underlying().relinearize(
      safe_static_cast<const Ciphertext &>(encrypted).underlying(),
      safe_static_cast<const RelinKeys &>(relin_keys).underlying(),
      *safe_static_cast<Ciphertext &>(destination).underlying_);
  }

  void Evaluator::mod_switch_to_next_inplace(api::Ciphertext &encrypted) const
  {
    underlying().mod_switch_to_next_inplace(*safe_static_cast<Ciphertext &>(encrypted).underlying_);
  }

  void Evaluator::mod_switch_to_next(const api::Ciphertext &encrypted, api::Ciphertext &destination) const
  {
    underlying().mod_switch_to_next(
      safe_static_cast<const Ciphertext &>(encrypted).underlying(),
      *safe_static_cast<Ciphertext &>(destination).underlying_);
  }

  void Evaluator::mod_switch_to_next_inplace(api::Plaintext &plain) const
  {
    underlying().mod_switch_to_next_inplace(*safe_static_cast<Plaintext &>(plain).underlying_);
  }

  void Evaluator::mod_switch_to_next(const api::Plaintext &plain, api::Plaintext &destination) const
  {
    underlying().mod_switch_to_next(
      safe_static_cast<const Plaintext &>(plain).underlying(), *safe_static_cast<Plaintext &>(destination).underlying_);
  }

  void Evaluator::rescale_to_next_inplace(api::Ciphertext &encrypted) const
  {
    underlying().rescale_to_next_inplace(*safe_static_cast<Ciphertext &>(encrypted).underlying_);
  }

  void Evaluator::rescale_to_next(const api::Ciphertext &encrypted, api::Ciphertext &destination) const
  {
    underlying().rescale_to_next(
      safe_static_cast<const Ciphertext &>(encrypted).underlying(),
      *safe_static_cast<Ciphertext &>(destination).underlying_);
  }

  void Evaluator::rotate_inplace(api::Ciphertext &encrypted, int steps, const api::GaloisKeys &galois_keys) const
  {
    seal::Ciphertext &seal_encrypted = *safe_static_cast<Ciphertext &>(encrypted).underlying_;
    const seal::GaloisKeys &seal_galois_keys = safe_static_cast<const GaloisKeys &>(galois_keys).underlying();
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

  void Evaluator::rotate(
    const api::Ciphertext &encrypted, int steps, const api::GaloisKeys &galois_keys, api::Ciphertext &destination) const
  {
    underlying().rotate_rows(
      safe_static_cast<const Ciphertext &>(encrypted).underlying(), steps,
      safe_static_cast<const GaloisKeys &>(galois_keys).underlying(),
      *safe_static_cast<Ciphertext &>(destination).underlying_);
  }

  void Evaluator::gen_symmetric_mask(
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
} // namespace seal_backend
} // namespace ufhe
