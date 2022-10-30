#pragma once

#include "ufhe/api/backendtype.hpp"
#include <cstdint>

namespace ufhe
{
class Config
{
public:
  Config() = delete;

  static inline void set_backend(BackendType backend) { backend_ = backend; }

  static inline BackendType backend() { return backend_; }

private:
  static BackendType backend_;
};
} // namespace ufhe
