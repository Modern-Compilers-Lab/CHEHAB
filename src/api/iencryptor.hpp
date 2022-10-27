#pragma once

#include "iciphertext.hpp"
#include "interface.hpp"
#include "iplaintext.hpp"

namespace ufhe
{
class IEncryptor : public Interface
{
public:
  virtual void encrypt(const IPlaintext &plain, ICiphertext &destination) const = 0;

  virtual void encrypt_symmetric(const IPlaintext &plain, ICiphertext &destination) const = 0;

  // TODO: allow creating seeded objects
};
} // namespace ufhe
