#pragma once

#include "ciphertext.hpp"
#include "plaintext.hpp"

namespace api
{
class Encryptor
{
  virtual void encrypt(const Plaintext &plain, Ciphertext &destination) const = 0;
};
} // namespace api
