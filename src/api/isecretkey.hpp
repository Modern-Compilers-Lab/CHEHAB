#pragma once

#include "api.hpp"

namespace ufhe
{
class ISecretKey
{
public:
  virtual ~ISecretKey() = default;

  virtual Backend backend() = 0;

  // TODO: Allow access to the key parms_id

  // TODO: Serialization support

protected:
  ISecretKey() = default;

  ISecretKey(const ISecretKey &copy) = default;

  ISecretKey &operator=(const ISecretKey &assign) = default;

  ISecretKey(ISecretKey &&source) = default;

  ISecretKey &operator=(ISecretKey &&assign) = default;
};
} // namespace ufhe
