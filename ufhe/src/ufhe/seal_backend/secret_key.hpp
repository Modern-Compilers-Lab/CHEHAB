#pragma once

#include "seal/seal.h"
#include "ufhe/api/secret_key.hpp"
#include <memory>

namespace ufhe
{
namespace seal_backend
{
  class SecretKey : public api::SecretKey
  {
  public:
    SecretKey() : underlying_(std::make_shared<seal::SecretKey>()) {}

    SecretKey(const seal::SecretKey &seal_sk)
      : underlying_(std::shared_ptr<seal::SecretKey>(&const_cast<seal::SecretKey &>(seal_sk), [](seal::SecretKey *) {}))
    {}

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline const seal::SecretKey &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::SecretKey> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
