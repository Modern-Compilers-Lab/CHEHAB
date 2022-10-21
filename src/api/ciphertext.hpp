#pragma once

#include "encryptioncontext.hpp"

namespace api
{
class Ciphertext
{
public:
  Ciphertext() {}

  Ciphertext(const EncryptionContext &context) { init(context); }

  // TODO: Constructor from context and a specefic parms_id_type from the modulus switching chain

  // TODO: resize

  // TODO: Allow access to a particular polynomial in the ciphertext data

  virtual std::size_t coeff_modulus_size() const = 0;

  virtual std::size_t poly_modulus_degree() const = 0;

  virtual std::size_t size() const = 0;

  virtual bool is_transparent() const = 0;

  virtual std::uint64_t &correction_factor() const = 0;

  virtual double &scale() const = 0;

  // TODO: Serialization support

private:
  virtual void init(const EncryptionContext &context) = 0;
};
} // namespace api
