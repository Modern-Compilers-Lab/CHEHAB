#pragma once

#include "ciphertext.hpp"
#include "plaintext.hpp"

namespace api
{
class Decryptor
{
  virtual void decrypt(const Ciphertext &encrypted, Plaintext &destination) = 0;
};
} // namespace api
