#pragma once

#include "ufhe/api/backendtype.hpp"

namespace ufhe
{
namespace api
{
  class Interface
  {
  public:
    virtual ~Interface() = default;

    virtual backend_type backend() const = 0;

  protected:
    Interface() = default;

    Interface(const Interface &copy) = default;

    Interface &operator=(const Interface &assign) = default;

    Interface(Interface &&source) = default;

    Interface &operator=(Interface &&assign) = default;
  };
} // namespace api
} // namespace ufhe
