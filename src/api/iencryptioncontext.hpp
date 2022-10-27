#pragma once

#include "api.hpp"
//#include "iencryptionparameters.hpp"

namespace ufhe
{
class IEncryptionContext
{
public:
  virtual ~IEncryptionContext() = default;

  virtual Backend backend() = 0;

  // virtual const IEncryptionParameters &get_parms() const = 0;

  // TODO: Virtual getters for context pre-computation data

protected:
  IEncryptionContext() = default;

  IEncryptionContext(const IEncryptionContext &copy) = default;

  IEncryptionContext &operator=(const IEncryptionContext &assign) = default;

  IEncryptionContext(IEncryptionContext &&source) = default;

  IEncryptionContext &operator=(IEncryptionContext &&assign) = default;
};
} // namespace ufhe
