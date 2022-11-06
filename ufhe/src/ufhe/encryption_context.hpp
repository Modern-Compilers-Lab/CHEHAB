#pragma once

#include "ufhe/api/encryption_context.hpp"
#include "ufhe/config.hpp"
#include "ufhe/encryption_params.hpp"
#include "ufhe/seal_backend/encryption_context.hpp"
#include <memory>

namespace ufhe
{
class EncryptionContext : public api::EncryptionContext
{
public:
  EncryptionContext(const EncryptionParams &params);

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  inline const api::EncryptionContext &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::EncryptionContext> underlying_;
};

} // namespace ufhe
