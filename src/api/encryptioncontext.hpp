#pragma once

#include "encryptionparameters.hpp"

namespace api
{
class EncryptionContext
{
public:
  EncryptionContext(const EncryptionParameters &parms) { init(parms); }

  virtual const EncryptionParameters &get_parms() const = 0;

  // TODO: Virtual getters for context pre-computation data

private:
  EncryptionContext() {}
  virtual void init(const EncryptionParameters &parms) = 0;
};
} // namespace api
