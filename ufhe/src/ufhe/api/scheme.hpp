#pragma once

#include "ufhe/api/abstract_type.hpp"

namespace ufhe
{
namespace api
{
  enum class scheme_type : std::uint8_t
  {
    none = 0x0,
    bfv = 0x1,
    bgv = 0x2,
    ckks = 0x3
  };

  class Scheme : public AbstractType
  {
  public:
    virtual api::scheme_type type() const = 0;
  };
} // namespace api
} // namespace ufhe
