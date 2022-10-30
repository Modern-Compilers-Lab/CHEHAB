#pragma once

#include "seal/seal.h"
#include "ufhe/api/irelinkeys.hpp"

namespace ufhe
{
namespace seal_backend
{
  class RelinKeys : public api::IRelinKeys
  {
  public:
    inline RelinKeys() {}

    inline api::BackendType backend() const override { return api::BackendType::seal; }

    inline std::size_t size() const override { return underlying_.size(); }

  private:
    seal::RelinKeys underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
