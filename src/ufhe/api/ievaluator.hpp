#pragma once

#include "ufhe/api/interface.hpp"
#include <vector>

namespace ufhe
{
namespace api
{
  class ICiphertext;
  class IPlaintext;

  class IEvaluator : public Interface
  {
  public:
    virtual void negate_inplace(ICiphertext &encrypted) const = 0;

    virtual void negate(const ICiphertext &encrypted, ICiphertext &destination) const = 0;

    virtual void add_inplace(ICiphertext &encypted1, const ICiphertext &encypted2) const = 0;

    virtual void add(const ICiphertext &encrypted1, const ICiphertext &encrypted2, ICiphertext &destination) const = 0;

    // virtual void add_many(const std::vector<ICiphertext> &encrypteds, ICiphertext &destination) const = 0;

    virtual void sub_inplace(ICiphertext &encrypted1, const ICiphertext &encrypted2) const = 0;

    virtual void sub(const ICiphertext &encrypted1, const ICiphertext &encrypted2, ICiphertext &destination) const = 0;

    virtual void multiply_inplace(ICiphertext &encrypted1, const ICiphertext &encrypted2) const = 0;

    virtual void multiply(
      const ICiphertext &encrypted1, const ICiphertext &encrypted2, ICiphertext &destination) const = 0;

    // virtual void multiply_many(const std::vector<ICiphertext> &encrypteds, const IRelinKeys &relin_keys, ICiphertext
    // &destination);

    virtual void square_inplace(ICiphertext &encrypted) const = 0;

    virtual void square(const ICiphertext &encrypted, ICiphertext &destination) const = 0;

    // virtual void exponentiate_inplace(ICiphertext &encrypted, std::uint64_t exponent, const IRelinKeys &relin_keys)
    // const = 0;

    // virtual void exponentiate(const ICiphertext &encrypted, std::uint64_t exponent, const IRelinKeys &relin_keys,
    // ICiphertext &destination) const = 0;

    virtual void add_plain_inplace(ICiphertext &encrypted, const IPlaintext &plain) const = 0;

    virtual void add_plain(const ICiphertext &encrypted, const IPlaintext &plain, ICiphertext &destination) const = 0;

    virtual void sub_plain_inplace(ICiphertext &encrypted, const IPlaintext &plain) const = 0;

    virtual void sub_plain(const ICiphertext &encrypted, const IPlaintext &plain, ICiphertext &destination) const = 0;

    virtual void multiply_plain_inplace(ICiphertext &encrypted, const IPlaintext &plain) const = 0;

    virtual void multiply_plain(
      const ICiphertext &encrypted, const IPlaintext &plain, ICiphertext &destination) const = 0;

    // virtual void relinearize_inplace(ICiphertext &encrypted, const IRelinKeys &relin_keys) const = 0;

    // virtual void relinearize(const ICiphertext &encrypted, const IRelinKeys &relin_keys, ICiphertext &destination)
    // const = 0;

    virtual void mod_switch_to_next_inplace(ICiphertext &encrypted) const = 0;

    virtual void mod_switch_to_next(const ICiphertext &encrypted, ICiphertext &destination) const = 0;

    virtual void mod_switch_to_next_inplace(IPlaintext &plain) const = 0;

    virtual void mod_switch_to_next(const IPlaintext &plain, IPlaintext &destination) const = 0;

    // TODO: mod_switch_to

    virtual void rescale_to_next(const ICiphertext &encrypted, ICiphertext &destination) const = 0;

    virtual void rescale_to_next_inplace(ICiphertext &encrypted) const = 0;

    // TODO: rescale_to

    // TODO: transform_to_ntt

    // TODO: transform_from_ntt

    // TODO: apply_galois

    // virtual void rotate_inplace(ICiphertext &encrypted, int steps, const IGaloisKeys &galois_keys) const = 0;

    // virtual void rotate(const ICiphertext &encrypted, int steps, const IGaloisKeys &galois_keys, ICiphertext
    // &destination) const = 0;

    // virtual void complex_conjugate_inplace(ICiphertext &encrypted, const IGaloisKeys &galois_keys) const = 0;

    // virtual void complex_conjugate(const ICiphertext &encrypted, const IGaloisKeys &galois_keys, ICiphertext
    // &destination) const = 0;
  };
} // namespace api
} // namespace ufhe
