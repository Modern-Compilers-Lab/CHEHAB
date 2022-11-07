#pragma once

#include "ufhe/api/galois_keys.hpp"
#include <memory>

namespace seal
{
class GaloisKeys;
} // namespace seal

namespace ufhe
{
namespace seal_backend
{
  class GaloisKeys : public api::GaloisKeys
  {
    friend class KeyGenerator;

  public:
    GaloisKeys();

    GaloisKeys(const GaloisKeys &copy);

    GaloisKeys &operator=(const GaloisKeys &assign);

    GaloisKeys(GaloisKeys &&source) = default;

    GaloisKeys &operator=(GaloisKeys &&assign) = default;

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    std::size_t size() const override;

    inline const seal::GaloisKeys &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::GaloisKeys> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
