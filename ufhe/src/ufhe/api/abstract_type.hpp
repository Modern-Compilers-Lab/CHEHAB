#pragma once

#include "ufhe/api/backend_type.hpp"
#include "ufhe/api/implementation_level.hpp"

namespace ufhe
{
namespace api
{
  class AbstractType
  {
  public:
    virtual ~AbstractType() = default;

    virtual backend_type backend() const = 0;

    virtual implementation_level level() const = 0;

  protected:
    AbstractType() = default;

    AbstractType(const AbstractType &copy) = default;

    AbstractType &operator=(const AbstractType &assign) = default;

    AbstractType(AbstractType &&source) = default;

    AbstractType &operator=(AbstractType &&assign) = default;
  };
} // namespace api
} // namespace ufhe
