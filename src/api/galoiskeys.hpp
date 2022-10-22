#pragma once

#include <cstddef>

namespace api
{
class GaloisKeys
{
public:
  GaloisKeys() { init(); }

  virtual ~GaloisKeys() {}

  virtual std::size_t size() const = 0;

  // TODO: Allow access to the keys parms_id

  // TODO: Serialization support

private:
  virtual void init() = 0;
};
} // namespace api
