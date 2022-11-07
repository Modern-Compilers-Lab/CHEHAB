#pragma once

#include "ufhe/api/abstract_type.hpp"

namespace ufhe
{
namespace api
{
  class EncryptionContext : public AbstractType
  {
    // virtual const EncryptionParams &get_params() const = 0;

    // TODO: Virtual getters for context pre-computation data
  };
} // namespace api
} // namespace ufhe
