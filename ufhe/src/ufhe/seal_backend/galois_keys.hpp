#pragma once

#include "seal/seal.h"
#include "ufhe/api/galois_keys.hpp"
#include <memory>

namespace ufhe
{
namespace seal_backend
{
  class GaloisKeys : public api::GaloisKeys
  {
    friend class KeyGenerator;

  public:
    GaloisKeys() : underlying_(std::make_shared<seal::GaloisKeys>()) {}

    GaloisKeys(const GaloisKeys &copy) : underlying_(std::make_shared<seal::GaloisKeys>(copy.underlying())) {}

    GaloisKeys &operator=(const GaloisKeys &assign)
    {
      underlying_ = std::make_shared<seal::GaloisKeys>(assign.underlying());
      return *this;
    }

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline std::size_t size() const override { return underlying().size(); }

    inline const seal::GaloisKeys &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::GaloisKeys> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
