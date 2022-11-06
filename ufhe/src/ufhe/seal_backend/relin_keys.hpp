#pragma once

#include "seal/seal.h"
#include "ufhe/api/relin_keys.hpp"
#include <memory>

namespace ufhe
{
namespace seal_backend
{
  class RelinKeys : public api::RelinKeys
  {
    friend class KeyGenerator;

  public:
    RelinKeys() : underlying_(std::make_shared<seal::RelinKeys>()) {}

    RelinKeys(const RelinKeys &copy) : underlying_(std::make_shared<seal::RelinKeys>(copy.underlying())) {}

    RelinKeys &operator=(const RelinKeys &assign)
    {
      underlying_ = std::make_shared<seal::RelinKeys>(assign.underlying());
      return *this;
    }

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    inline std::size_t size() const override { return underlying().size(); }

    inline const seal::RelinKeys &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::RelinKeys> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
