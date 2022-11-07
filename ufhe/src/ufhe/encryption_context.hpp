#pragma once

#include "ufhe/api/encryption_context.hpp"
#include <memory>

namespace ufhe
{
class EncryptionParams;

class EncryptionContext : public api::EncryptionContext
{
public:
  explicit EncryptionContext(const EncryptionParams &params);

  EncryptionContext(const EncryptionContext &copy);

  EncryptionContext &operator=(const EncryptionContext &assign);

  EncryptionContext(EncryptionContext &&source) = default;

  EncryptionContext &operator=(EncryptionContext &&assign) = default;

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline api::implementation_level level() const override { return api::implementation_level::high_level; }

  inline const api::EncryptionContext &underlying() const { return *underlying_; }

private:
  std::shared_ptr<api::EncryptionContext> underlying_;
};
} // namespace ufhe
