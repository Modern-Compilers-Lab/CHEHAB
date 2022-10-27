#pragma once

#include "api.hpp"
#include "imodulus.hpp"

namespace ufhe
{
class ICoeffModulus
{
public:
  virtual ~ICoeffModulus() = default;

  virtual Backend backend() = 0;

  virtual IModulus::vector value() const = 0;

  // TODO: Serialization support

protected:
  ICoeffModulus() = default;

  ICoeffModulus(const ICoeffModulus &copy) = default;

  ICoeffModulus &operator=(const ICoeffModulus &assign) = default;

  ICoeffModulus(ICoeffModulus &&source) = default;

  ICoeffModulus &operator=(ICoeffModulus &&assign) = default;
};
} // namespace ufhe
