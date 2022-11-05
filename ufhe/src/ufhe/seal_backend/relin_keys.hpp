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

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline std::size_t size() const override { return underlying().size(); }

    inline const seal::RelinKeys &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::RelinKeys> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
