#pragma once

#include "ufhe/api/backend_type.hpp"
#include "ufhe/api/implementation_level.hpp"
#include <stdexcept>

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

    template <class T>
    inline void check_strict_compatibility(const T &t) const
    {
      if (backend() != t.backend())
        throw std::invalid_argument("argument with different backend");
      if (level() != t.level())
        throw std::invalid_argument("argument with different implementation level");
    }
  };
} // namespace api
} // namespace ufhe
