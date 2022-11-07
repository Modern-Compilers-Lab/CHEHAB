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

    template <class TConcrete, class TAbstract>
    const TConcrete &safe_static_cast(const TAbstract &arg) const
    {
      if (backend() != arg.backend())
        throw std::invalid_argument("argument with different backend");
      if (level() != arg.level())
        throw std::invalid_argument("argument with different implementation level");
      return static_cast<const TConcrete &>(arg);
    }

    template <class TConcrete, class TAbstract>
    TConcrete &safe_static_cast(TAbstract &arg) const
    {
      if (backend() != arg.backend())
        throw std::invalid_argument("argument with different backend");
      if (level() != arg.level())
        throw std::invalid_argument("argument with different implementation level");
      return static_cast<TConcrete &>(arg);
    }
  };
} // namespace api
} // namespace ufhe
