#pragma once

#include "interface.hpp"
#include <cstddef>

namespace ufhe
{
class IRelinKeys : public Interface
{
public:
  virtual std::size_t size() const = 0;

  // TODO: Allow access to the keys parms_id

  // TODO: Serialization support
};
} // namespace ufhe
