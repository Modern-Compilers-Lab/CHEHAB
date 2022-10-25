#pragma once

#include "modulus.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace ufhe
{
class SchemeType
{
public:
  using ptr = std::unique_ptr<SchemeType>;

  static ptr create(Backend backend, std::uint8_t scheme_id);

  static inline ptr create(std::uint8_t scheme_id) { return create(Backend::none, scheme_id); }

  virtual ~SchemeType() = default;

protected:
  SchemeType() = default;

  SchemeType(const SchemeType &) = default;

  SchemeType &operator=(const SchemeType &) = default;

  SchemeType(SchemeType &&) = default;

  SchemeType &operator=(SchemeType &&) = default;
};

class EncryptionParameters
{
public:
  using ptr = std::unique_ptr<EncryptionParameters>;

  static ptr create(Backend backend, const SchemeType::ptr &scheme);

  static inline ptr create(const SchemeType::ptr &scheme) { return create(Backend::none, scheme); }

  virtual ~EncryptionParameters() {}

  virtual void set_poly_modulus_degree(std::size_t poly_modulus_degree) = 0;

  virtual void set_coeff_modulus(const Modulus::vector &coeff_modulus) = 0;

  virtual void set_plain_modulus(const Modulus::ptr &plain_modulus) = 0;

  // TODO: Virtual setter for the random number generator factory

  virtual const SchemeType &scheme() const = 0;

  virtual std::size_t poly_modulus_degree() const = 0;

  virtual const Modulus::vector &coeff_modulus() const = 0;

  virtual const Modulus &plain_modulus() const = 0;

  // TODO: Virtual getter for the random number generator factory

  // TODO: Serialization support

protected:
  EncryptionParameters() = default;

  EncryptionParameters(const EncryptionParameters &) = default;

  EncryptionParameters &operator=(const EncryptionParameters &) = default;

  EncryptionParameters(EncryptionParameters &&) = default;

  EncryptionParameters &operator=(EncryptionParameters &&) = default;
};
} // namespace ufhe
