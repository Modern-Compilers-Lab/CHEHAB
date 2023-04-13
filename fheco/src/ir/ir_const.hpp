#pragma once

#include <cstdint>
#include <variant>
#include <vector>

namespace fhecompiler
{
namespace ir
{
  using ScalarValue = std::variant<std::uint64_t, std::int64_t>;

  using VectorValue = std::variant<std::vector<std::uint64_t>, std::vector<std::int64_t>>;

  using ConstantValue = std::variant<VectorValue, ScalarValue>;

  enum class TagTableEntryType
  {
    constant,
    input,
    temp,
    output
  };

  // overload pattern
  template <class... Ts>
  struct overloaded : Ts...
  {
    using Ts::operator()...;
  };
  template <class... Ts>
  overloaded(Ts...) -> overloaded<Ts...>;
} // namespace ir
} // namespace fhecompiler
