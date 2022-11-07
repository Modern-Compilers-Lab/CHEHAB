#pragma once

#include "ufhe/api/plaintext.hpp"
#include <memory>

namespace seal
{
class Plaintext;
} // namespace seal

namespace ufhe
{
namespace seal_backend
{
  class Plaintext : public api::Plaintext
  {
    friend class BatchEncoder;
    friend class Decryptor;
    friend class Evaluator;

  public:
    Plaintext();

    explicit Plaintext(std::size_t coeff_count);

    explicit Plaintext(const std::string &hex_poly);

    Plaintext(const Plaintext &copy);

    Plaintext &operator=(const Plaintext &assign);

    Plaintext(Plaintext &&source) = default;

    Plaintext &operator=(Plaintext &&assign) = default;

    inline api::backend_type backend() const override { return api::backend_type::seal; }

    inline api::implementation_level level() const override { return api::implementation_level::low_level; }

    void resize(std::size_t coeff_count) override;

    void set_zero(std::size_t start_coeff, std::size_t length) override;

    std::size_t capacity() const override;

    std::size_t coeff_count() const override;

    std::string to_string() const override;

    bool operator==(const api::Plaintext &compare) const override;

    bool operator!=(const api::Plaintext &compare) const override;

    inline const seal::Plaintext &underlying() const { return *underlying_; }

  private:
    std::shared_ptr<seal::Plaintext> underlying_;
  };
} // namespace seal_backend
} // namespace ufhe
