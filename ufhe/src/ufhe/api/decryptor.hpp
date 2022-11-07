#pragma once

#include "ufhe/api/abstract_type.hpp"

namespace ufhe
{
namespace api
{
  class Ciphertext;
  class Plaintext;

  class Decryptor : public AbstractType
  {
  public:
    virtual void decrypt(const Ciphertext &encrypted, Plaintext &destination) const = 0;

    virtual int invariant_noise_budget(const Ciphertext &encrypted) const = 0;
  };
} // namespace api
} // namespace ufhe
