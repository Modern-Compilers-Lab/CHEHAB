#pragma once

#include <cstddef>
#include <cstdint>
#include <variant>
#include <vector>

namespace fhecompiler
{
namespace ir
{
  using VectorValue = std::variant<std::vector<std::uint64_t>, std::vector<std::int64_t>>;

  using ScalarValue = std::variant<std::uint64_t, std::int64_t>;

  using ConstantValue = std::variant<VectorValue, ScalarValue>;

  // overload pattern
  template <class... Ts>
  struct overloaded : Ts...
  {
    using Ts::operator()...;
  };
  template <class... Ts>
  overloaded(Ts...) -> overloaded<Ts...>;

  // https://github.com/HowardHinnant/hash_append/issues/7#issuecomment-629414712
  template <typename T>
  inline void hash_combine(std::size_t &seed, const T &val)
  {
    seed ^= std::hash<T>{}(val) + 0x9e3779b97f4a7c15ULL + (seed << 12) + (seed >> 4);
  }

  struct ConstantValueHash
  {
    size_t vector_size_;

    ConstantValueHash(size_t vector_size) : vector_size_{vector_size} {}

    std::size_t operator()(const ConstantValue &v) const;
  };

  enum class TermQualif
  {
    temp,
    in,
    in_out,
    const_,
    out
  };

  enum class Scheme
  {
    none,
    bfv
  };

  enum class Backend
  {
    none,
    seal
  };

  enum class SecurityLevel
  {
    none,
    tc128,
    tc192,
    tc256
  };
} // namespace ir
} // namespace fhecompiler
