#pragma once

#include "ciphertext.hpp"
#include "context.hpp"

namespace api
{
class Evaluator
{
public:
  virtual void add_inplace(Ciphertext &encypted1, const Ciphertext &encypted2) = 0;
};
} // namespace api
