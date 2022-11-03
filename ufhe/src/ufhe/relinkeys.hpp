#pragma once

#include "ufhe/api/irelinkeys.hpp"
#include "ufhe/config.hpp"
#include "ufhe/seal_backend/relinkeys.hpp"

namespace ufhe
{
class RelinKeys : public api::IRelinKeys
{
  friend class KeyGenerator;

public:
  RelinKeys()
  {
    switch (Config::backend())
    {
    case api::backend_type::seal:
      underlying_ = new seal_backend::RelinKeys();
      break;

    case api::backend_type::none:
      throw std::invalid_argument("no backend is selected");
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }
  RelinKeys(const RelinKeys &copy) = delete;

  RelinKeys &operator=(const RelinKeys &assign) = delete;

  ~RelinKeys() { delete underlying_; }

  inline api::backend_type backend() const override { return underlying().backend(); }

  inline std::size_t size() const override { return underlying().size(); }

private:
  inline IRelinKeys &underlying() const { return *underlying_; }

  IRelinKeys *underlying_;
};
} // namespace ufhe
