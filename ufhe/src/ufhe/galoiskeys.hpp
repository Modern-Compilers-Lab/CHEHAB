#pragma once

#include "ufhe/api/igaloiskeys.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/galoiskeys.hpp"

namespace ufhe
{
class GaloisKeys : public api::IGaloisKeys
{
  friend class KeyGenerator;

public:
  GaloisKeys()
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::GaloisKeys();
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }
  GaloisKeys(const GaloisKeys &copy) = delete;

  GaloisKeys &operator=(const GaloisKeys &assign) = delete;

  ~GaloisKeys() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline std::size_t size() const override { return underlying().size(); }

private:
  inline IGaloisKeys &underlying() const { return *underlying_; }

  IGaloisKeys *underlying_;
};
} // namespace ufhe
