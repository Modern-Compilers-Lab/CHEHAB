#pragma once

#include "ufhe/api/interface.hpp"

namespace ufhe
{
namespace api
{
  class ICiphertext;
  class IPlaintext;

  class IDecryptor : public Interface
  {
  public:
    virtual void decrypt(const ICiphertext &encrypted, IPlaintext &destination) = 0;

    virtual int invariant_noise_budget(const ICiphertext &encrypted) = 0;
  };
} // namespace api
} // namespace ufhe
