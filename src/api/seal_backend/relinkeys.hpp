#pragma once

#include "../irelinkeys.hpp"
#include "seal/seal.h"

namespace ufhe
{
namespace seal_backend
{
  class RelinKeys : public IRelinKeys
  {
  public:
    inline RelinKeys() {}

    inline Backend backend() const override { return Backend::seal; }

    inline std::size_t size() const override { return underlying_.size(); }

  private:
    seal::RelinKeys underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
