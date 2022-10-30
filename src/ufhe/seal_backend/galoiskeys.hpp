#pragma once

#include "seal/seal.h"
#include "ufhe/api/igaloiskeys.hpp"

namespace ufhe
{
namespace seal_backend
{
  class GaloisKeys : public api::IGaloisKeys
  {
  public:
    inline GaloisKeys() {}

    inline api::BackendType backend() const override { return api::BackendType::seal; }

    inline std::size_t size() const override { return underlying_.size(); }

  private:
    seal::GaloisKeys underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
