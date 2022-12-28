#pragma once

#include "ufhe/api/abstract_type.hpp"
#include "ufhe/api/modulus.hpp"

namespace ufhe
{
namespace api
{
  class CoeffModulus : public AbstractType
  {
  public:
    virtual Modulus::vector value() const = 0;

    // TODO: Serialization support
  };
} // namespace api
} // namespace ufhe
