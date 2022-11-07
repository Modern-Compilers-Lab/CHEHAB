#include "ufhe/evaluator.hpp"
#include "ufhe/ciphertext.hpp"
#include "ufhe/encryption_context.hpp"
#include "ufhe/galois_keys.hpp"
#include "ufhe/plaintext.hpp"
#include "ufhe/relin_keys.hpp"
#include "ufhe/seal_backend/evaluator.hpp"

namespace ufhe
{
Evaluator::Evaluator(const EncryptionContext &context)
{
  switch (context.backend())
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::Evaluator>(
      static_cast<const seal_backend::EncryptionContext &>(context.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

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
    safe_static_cast<const Plaintext &>(plain).underlying(), *safe_static_cast<Ciphertext &>(destination).underlying_);
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
    safe_static_cast<const Plaintext &>(plain).underlying(), *safe_static_cast<Ciphertext &>(destination).underlying_);
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
    safe_static_cast<const Plaintext &>(plain).underlying(), *safe_static_cast<Ciphertext &>(destination).underlying_);
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
  underlying().rotate_inplace(
    *safe_static_cast<Ciphertext &>(encrypted).underlying_, steps,
    safe_static_cast<const GaloisKeys &>(galois_keys).underlying());
}

void Evaluator::rotate(
  const api::Ciphertext &encrypted, int steps, const api::GaloisKeys &galois_keys, api::Ciphertext &destination) const
{
  underlying().rotate(
    safe_static_cast<const Ciphertext &>(encrypted).underlying(), steps,
    safe_static_cast<const GaloisKeys &>(galois_keys).underlying(),
    *safe_static_cast<Ciphertext &>(destination).underlying_);
}
} // namespace ufhe
