#pragma once

#include "ufhe/api/backend_type.hpp"

namespace ufhe
{
class Config
{
public:
  Config() = delete;

  static inline void set_backend(api::backend_type backend) { backend_ = backend; }

  static inline api::backend_type backend() { return backend_; }

private:
  static api::backend_type backend_;
};
} // namespace ufhe
