#pragma once

#include "ufhe/api/interface.hpp"

namespace ufhe
{
namespace api
{
  class IEncryptionContext : public Interface
  {
    // virtual const IEncryptionParameters &get_parms() const = 0;

    // TODO: Virtual getters for context pre-computation data
  };
} // namespace api
} // namespace ufhe
