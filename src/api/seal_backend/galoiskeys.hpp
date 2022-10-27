#pragma once

#include "../igaloiskeys.hpp"
#include "seal/seal.h"

namespace ufhe
{
namespace seal_backend
{
  class GaloisKeys : public IGaloisKeys
  {
  public:
    inline GaloisKeys() {}

    inline Backend backend() const override { return Backend::seal; }

    inline std::size_t size() const override { return underlying_.size(); }

  private:
    seal::GaloisKeys underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
