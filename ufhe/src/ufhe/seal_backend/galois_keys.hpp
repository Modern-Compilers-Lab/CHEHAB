#pragma once

#include "seal/seal.h"
#include "ufhe/api/galois_keys.hpp"

namespace ufhe
{
namespace seal_backend
{
  class GaloisKeys : public api::GaloisKeys
  {
  public:
    GaloisKeys() {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline std::size_t size() const override { return underlying_.size(); }

  private:
    seal::GaloisKeys underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
