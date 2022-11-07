#pragma once

#include "ufhe/api/encryption_context.hpp"
#include <memory>

namespace seal
{
class SEALContext;
} // namespace seal

namespace ufhe
{
namespace seal_backend
{
  class EncryptionParams;

  class EncryptionContext : public api::EncryptionContext
  {
  public:
    explicit EncryptionContext(const EncryptionParams &params);

    EncryptionContext(const EncryptionContext &copy) = default;

    EncryptionContext &operator=(const EncryptionContext &assign) = default;

    EncryptionContext(EncryptionContext &&source) = default;

    EncryptionContext &operator=(EncryptionContext &&assign) = default;

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    inline const seal::SEALContext &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::SEALContext> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
