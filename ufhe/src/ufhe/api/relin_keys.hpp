#pragma once

#include "ufhe/api/abstract_type.hpp"
#include <cstddef>

namespace ufhe
{
namespace api
{
  class RelinKeys : public AbstractType
  {
  public:
    virtual std::size_t size() const = 0;

    // TODO: Allow access to the keys params_id

    // TODO: Serialization support
  };
} // namespace api
} // namespace ufhe
