#pragma once

#include "api.hpp"

namespace ufhe
{
class IPublicKey
{
public:
  virtual ~IPublicKey() = default;

  virtual Backend backend() = 0;

  // TODO: Allow access to the key parms_id

  // TODO: Serialization support

protected:
  IPublicKey() = default;

  IPublicKey(const IPublicKey &copy) = default;

  IPublicKey &operator=(const IPublicKey &assign) = default;

  IPublicKey(IPublicKey &&source) = default;

  IPublicKey &operator=(IPublicKey &&assign) = default;
};
} // namespace ufhe
