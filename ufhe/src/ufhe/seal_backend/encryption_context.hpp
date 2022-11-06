#pragma once

#include "seal/seal.h"
#include "ufhe/api/encryption_context.hpp"
#include "ufhe/seal_backend/encryption_params.hpp"
#include <memory>

namespace ufhe
{
namespace seal_backend
{
  class EncryptionContext : public api::EncryptionContext
  {
  public:
    EncryptionContext(const EncryptionParams &params)
      : underlying_(std::make_shared<seal::SEALContext>(params.underlying()))
    {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    inline const seal::SEALContext &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::SEALContext> underlying_;
  };

} // namespace seal_backend
} // namespace ufhe
