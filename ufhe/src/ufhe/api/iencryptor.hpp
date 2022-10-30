#pragma once

#include "ufhe/api/interface.hpp"

namespace ufhe
{
namespace api
{
  class ICiphertext;
  class IPlaintext;

  class IEncryptor : public Interface
  {
  public:
    virtual void encrypt(const IPlaintext &plain, ICiphertext &destination) const = 0;

    virtual void encrypt_symmetric(const IPlaintext &plain, ICiphertext &destination) const = 0;

    // TODO: allow creating seeded objects
  };
} // namespace api
} // namespace ufhe
