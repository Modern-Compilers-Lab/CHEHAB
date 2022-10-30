#pragma once

#include "ufhe/api/imodulus.hpp"
#include "ufhe/api/interface.hpp"

namespace ufhe
{
namespace api
{
  class ICoeffModulus : public Interface
  {
  public:
    virtual IModulus::vector value() const = 0;

    // TODO: Serialization support
  };
} // namespace api
} // namespace ufhe
