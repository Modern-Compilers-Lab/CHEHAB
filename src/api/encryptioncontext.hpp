#pragma once

#include "api.hpp"
#include "encryptionparameters.hpp"
#include "iencryptioncontext.hpp"
#include "seal_backend/encryptioncontext.hpp"

namespace ufhe
{
class EncryptionContext : public IEncryptionContext
{
  friend class KeyGenerator;

public:
  inline EncryptionContext(Backend backend, const IEncryptionParameters &parms)
  {
    if (backend == Backend::none)
      backend = API::default_backend();
    switch (backend)
    {
    case Backend::seal:
      underlying_ = new seal_backend::EncryptionContext(dynamic_cast<const EncryptionParameters &>(parms).underlying());
      break;

    default:
      throw std::invalid_argument("unsupported backend");
      break;
    }
  }

  inline EncryptionContext(const IEncryptionParameters &parms) : EncryptionContext(Backend::none, parms) {}

  EncryptionContext(const EncryptionContext &copy) = delete;

  EncryptionContext &operator=(const EncryptionContext &assign) = delete;

  inline ~EncryptionContext() { delete underlying_; }

  inline Backend backend() { return underlying().backend(); }

private:
  inline IEncryptionContext &underlying() const { return *underlying_; }

  IEncryptionContext *underlying_;
};

} // namespace ufhe
