#pragma once

#include "ciphertext.hpp"
#include "encryptioncontext.hpp"
#include "galoiskeys.hpp"
#include "plaintext.hpp"
#include "relinkeys.hpp"

namespace api
{
class Evaluator
{
public:
  Evaluator(const EncryptionContext &context) { init(context); }

  virtual void negate_inplace(Ciphertext &encrypted) const = 0;

  virtual void negate(const Ciphertext &encrypted, Ciphertext &destination) const = 0;

  virtual void add_inplace(Ciphertext &encypted1, const Ciphertext &encypted2) const = 0;

  virtual void add(const Ciphertext &encrypted1, const Ciphertext &encrypted2, Ciphertext &destination) const = 0;

  virtual void add_many(const std::vector<Ciphertext> &encrypteds, Ciphertext &destination) const = 0;

  virtual void sub_inplace(Ciphertext &encrypted1, const Ciphertext &encrypted2) const = 0;

  virtual void sub(const Ciphertext &encrypted1, const Ciphertext &encrypted2, Ciphertext &destination) const = 0;

  virtual void multiply_inplace(Ciphertext &encrypted1, const Ciphertext &encrypted2) const = 0;

  virtual void multiply(const Ciphertext &encrypted1, const Ciphertext &encrypted2, Ciphertext &destination) const = 0;

  virtual void multiply_many(
    const std::vector<Ciphertext> &encrypteds, const RelinKeys &relin_keys, Ciphertext &destination);

  virtual void square_inplace(Ciphertext &encrypted) const = 0;

  virtual void square(const Ciphertext &encrypted, Ciphertext &destination) const = 0;

  virtual void exponentiate_inplace(
    Ciphertext &encrypted, std::uint64_t exponent, const RelinKeys &relin_keys) const = 0;

  virtual void exponentiate(
    const Ciphertext &encrypted, std::uint64_t exponent, const RelinKeys &relin_keys,
    Ciphertext &destination) const = 0;

  virtual void add_plain_inplace(Ciphertext &encrypted, const Plaintext &plain) const = 0;

  virtual void add_plain(const Ciphertext &encrypted, const Plaintext &plain, Ciphertext &destination) const = 0;

  virtual void sub_plain_inplace(Ciphertext &encrypted, const Plaintext &plain) const = 0;

  virtual void sub_plain(const Ciphertext &encrypted, const Plaintext &plain, Ciphertext &destination) const = 0;

  virtual void multiply_plain_inplace(Ciphertext &encrypted, const Plaintext &plain) const = 0;

  virtual void multiply_plain(const Ciphertext &encrypted, const Plaintext &plain, Ciphertext &destination) const = 0;

  virtual void relinearize_inplace(Ciphertext &encrypted, const RelinKeys &relin_keys) const = 0;

  virtual void relinearize(const Ciphertext &encrypted, const RelinKeys &relin_keys, Ciphertext &destination) const = 0;

  virtual void mod_switch_to_next_inplace(Ciphertext &encrypted) const = 0;

  virtual void mod_switch_to_next(const Ciphertext &encrypted, Ciphertext &destination) const = 0;

  virtual void mod_switch_to_next_inplace(Plaintext &plain) const = 0;

  virtual void mod_switch_to_next(const Plaintext &plain, Plaintext &destination) const = 0;

  // TODO: mod_switch_to

  virtual void rescale_to_next(const Ciphertext &encrypted, Ciphertext &destination) const = 0;

  virtual void rescale_to_next_inplace(Ciphertext &encrypted) const = 0;

  // TODO: rescale_to

  // TODO: transform_to_ntt

  // TODO: transform_from_ntt

  // TODO: apply_galois

  virtual void rotate_inplace(Ciphertext &encrypted, int steps, const GaloisKeys &galois_keys) const = 0;

  virtual void rotate(
    const Ciphertext &encrypted, int steps, const GaloisKeys &galois_keys, Ciphertext &destination) const = 0;

  virtual void complex_conjugate_inplace(Ciphertext &encrypted, const GaloisKeys &galois_keys) const = 0;

  virtual void complex_conjugate(
    const Ciphertext &encrypted, const GaloisKeys &galois_keys, Ciphertext &destination) const = 0;

private:
  Evaluator() {}

  virtual void init(const EncryptionContext &context) = 0;
};
} // namespace api
