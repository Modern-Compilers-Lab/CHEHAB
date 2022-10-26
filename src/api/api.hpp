#pragma once

#include <cstdint>

namespace ufhe
{

enum class Backend : std::uint8_t
{
  none = 0x0,

  seal = 0x1
};

class API
{
public:
  API() = delete;

  static inline void set_default_backend(Backend backend) { default_backend_ = backend; }

  static inline Backend default_backend() { return default_backend_; }

private:
  static Backend default_backend_;
};
} // namespace ufhe
