#pragma once

#include <cstdint>
#include <string>

namespace fhecompiler
{
class Scalar
{
private:
  std::string label; // symbol

  static size_t scalar_id;

public:
  Scalar() {}

  Scalar(int64_t data);

  Scalar(int data) : Scalar((int64_t)data) {}

  Scalar(uint64_t data);

  Scalar(double data);

  Scalar(const Scalar &) = default;

  Scalar &operator=(const Scalar &) = default;

  Scalar(Scalar &&) = default;

  Scalar &operator=(Scalar &&) = default;

  std::string get_label() const { return this->label; }

  void set_label(const std::string &label) { this->label = label; }

  void set_new_label();
};
} // namespace fhecompiler
