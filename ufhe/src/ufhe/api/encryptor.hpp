#pragma once

#include "ufhe/api/abstract_type.hpp"

namespace ufhe
{
namespace api
{
  class Ciphertext;
  class Plaintext;

  class Encryptor : public AbstractType
  {
  public:
    virtual void encrypt(const Plaintext &plain, Ciphertext &destination) const = 0;

    virtual void encrypt_symmetric(const Plaintext &plain, Ciphertext &destination) const = 0;

    // TODO: allow creating seeded objects
  };
} // namespace api
} // namespace ufhe
