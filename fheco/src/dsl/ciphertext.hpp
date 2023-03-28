#pragma once

#include <string>

namespace fhecompiler
{
class Ciphertext
{
private:
  std::string label; // symbol

  static size_t ciphertext_id;

public:
  Ciphertext() {}

  Ciphertext(const std::string &tag);

  Ciphertext(const Ciphertext &) = default;

  Ciphertext &operator=(const Ciphertext &) = default;

  Ciphertext(Ciphertext &&) = default;

  Ciphertext &operator=(Ciphertext &&) = default;

  void set_output(const std::string &tag);

  std::string get_label() const { return this->label; }

  void set_label(const std::string &label) { this->label = label; }

  std::string get_term_tag();

  void set_new_label();

  bool is_output() const;
};
} // namespace fhecompiler
