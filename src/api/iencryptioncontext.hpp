#pragma once

#include "interface.hpp"

namespace ufhe
{
class IEncryptionContext : public virtual Interface
{
public:
  // virtual const IEncryptionParameters &get_parms() const = 0;

  // TODO: Virtual getters for context pre-computation data
};
} // namespace ufhe
