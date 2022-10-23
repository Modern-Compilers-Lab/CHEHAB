#pragma once

#include "backend.hpp"
#include <cstdint>

namespace api
{
class API
{
public:
  static inline void set_default_backend(Backend backend) { backend_ = backend; }

  static inline Backend default_backend() { return backend_; }

private:
  static Backend backend_;
};
} // namespace api
