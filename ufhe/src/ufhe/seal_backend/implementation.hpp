#pragma once

#include "../interface.hpp"

namespace ufhe
{
namespace seal_backend
{
  class Implementation : public virtual Interface
  {
  public:
    inline Backend backend() const override { return Backend::seal; }
  };
} // namespace seal_backend
} // namespace ufhe
