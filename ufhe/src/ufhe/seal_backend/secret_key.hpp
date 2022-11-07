#pragma once

#include "ufhe/api/secret_key.hpp"
#include <memory>

namespace seal
{
class SecretKey;
} // namespace seal

namespace ufhe
{
namespace seal_backend
{
  class SecretKey : public api::SecretKey
  {
    friend class KeyGenerator;

  public:
    SecretKey();

    SecretKey(const SecretKey &copy);

    SecretKey &operator=(const SecretKey &assign);

    SecretKey(SecretKey &&source) = default;

    SecretKey &operator=(SecretKey &&assign) = default;

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    inline const seal::SecretKey &underlying() const { return *underlying_; }

  private:
    explicit SecretKey(const seal::SecretKey &secret_key);

    std::shared_ptr<seal::SecretKey> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
