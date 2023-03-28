#pragma once

#include <string>
#include <vector>

namespace fhecompiler
{

class Plaintext
{
private:
  std::string label; // symbol

  static std::size_t plaintext_id;

public:
  Plaintext() {}

  Plaintext(const std::vector<std::int64_t> &data);

  Plaintext(const std::vector<std::uint64_t> &data);

  Plaintext(const std::vector<double> &data);

  Plaintext(const std::string &tag);

  Plaintext(const Plaintext &) = default;

  Plaintext &operator=(const Plaintext &) = default;

  Plaintext &operator=(Plaintext &&) = default;

  Plaintext(Plaintext &&pt_move) = default;

  void set_output(const std::string &tag);

  std::string get_label() const { return this->label; }

  void set_label(const std::string &label) { this->label = label; }

  void set_new_label();
};

} // namespace fhecompiler
