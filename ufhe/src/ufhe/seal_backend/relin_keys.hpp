#pragma once

#include "ufhe/api/relin_keys.hpp"
#include <memory>

namespace seal
{
class RelinKeys;
} // namespace seal

namespace ufhe
{
namespace seal_backend
{
  class RelinKeys : public api::RelinKeys
  {
    friend class KeyGenerator;

  public:
    RelinKeys();

    RelinKeys(const RelinKeys &copy);

    RelinKeys &operator=(const RelinKeys &assign);

    RelinKeys(RelinKeys &&source) = default;

    RelinKeys &operator=(RelinKeys &&assign) = default;

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    std::size_t size() const override;

    inline const seal::RelinKeys &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::RelinKeys> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
