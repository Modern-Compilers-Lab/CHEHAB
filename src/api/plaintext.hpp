#pragma once

#include <cstddef>
#include <string>

namespace api
{
class Plaintext
{
public:
  Plaintext() {}

  Plaintext(std::size_t coeff_count) {}

  Plaintext(const std::string &hex_poly) { init(hex_poly); }

  virtual void resize(std::size_t coeff_count) = 0;

  virtual void set_zero(std::size_t start_coeff, std::size_t length) = 0;

  virtual std::size_t capacity() const = 0;

  virtual std::size_t coeff_count() const = 0;

  virtual std::string to_string() const = 0;

  virtual bool operator==(const Plaintext &compare) const = 0;

  virtual bool operator!=(const Plaintext &compare) const = 0;

  // TODO: Allow access to specific coefficient of the plaintext polynomial

  // TODO: Serialization support

private:
  virtual void init(std::size_t coeff_count) = 0;

  virtual void init(const std::string &hex_poly) = 0;
};
} // namespace api
