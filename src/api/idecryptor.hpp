#pragma once

#include "iciphertext.hpp"
#include "interface.hpp"
#include "iplaintext.hpp"

namespace ufhe
{
class IDecryptor : public virtual Interface
{
public:
  virtual void decrypt(const ICiphertext &encrypted, IPlaintext &destination) = 0;

  virtual int invariant_noise_budget(const ICiphertext &encrypted) = 0;
};
} // namespace ufhe
