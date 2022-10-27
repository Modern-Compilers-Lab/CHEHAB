#pragma once

#include "api.hpp"

namespace ufhe
{
class Interface
{
public:
  virtual ~Interface() = default;

  virtual Backend backend() const = 0;

protected:
  Interface() = default;

  Interface(const Interface &copy) = default;

  Interface &operator=(const Interface &assign) = default;

  Interface(Interface &&source) = default;

  Interface &operator=(Interface &&assign) = default;
};
} // namespace ufhe
