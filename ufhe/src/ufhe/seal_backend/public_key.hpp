#pragma once

#include "ufhe/api/public_key.hpp"
#include <memory>

namespace seal
{
class PublicKey;
} // namespace seal

namespace ufhe
{
namespace seal_backend
{
  class PublicKey : public api::PublicKey
  {
    friend class KeyGenerator;

  public:
    PublicKey();

    PublicKey(const PublicKey &copy);

    PublicKey &operator=(const PublicKey &assign);

    PublicKey(PublicKey &&source) = default;

    PublicKey &operator=(PublicKey &&assign) = default;

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    inline const seal::PublicKey &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::PublicKey> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
