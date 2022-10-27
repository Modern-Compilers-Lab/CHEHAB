#pragma once

#include "../iplaintext.hpp"
#include "implementation.hpp"
#include "seal/seal.h"

namespace ufhe
{
namespace seal_backend
{
  class Plaintext : public Implementation, public IPlaintext
  {
    friend class BatchEncoder;
    friend class Encryptor;
    friend class Decryptor;
    friend class Evaluator;

  public:
    inline Plaintext() : Plaintext(new seal::Plaintext(), true) {}

    inline Plaintext(std::size_t coeff_count) : Plaintext(new seal::Plaintext(coeff_count), true) {}

    inline Plaintext(const std::string &hex_poly) : Plaintext(new seal::Plaintext(hex_poly), true) {}

    inline Plaintext(const Plaintext &copy) : Plaintext(copy.underlying_, false) {}

    Plaintext &operator=(const Plaintext &assign) = delete;

    inline ~Plaintext()
    {
      if (is_owner_)
        delete underlying_;
    }

    inline void resize(std::size_t coeff_count) override { underlying().resize(coeff_count); }

    inline void set_zero(std::size_t start_coeff, std::size_t length) override
    {
      underlying().set_zero(start_coeff, length);
    }

    inline std::size_t capacity() const override { return underlying().capacity(); }

    inline std::size_t coeff_count() const override { return underlying().coeff_count(); }

    inline std::string to_string() const override { return underlying().to_string(); }

    inline bool operator==(const IPlaintext &compare) const override
    {
      return underlying() == dynamic_cast<const Plaintext &>(compare).underlying();
    }

    inline bool operator!=(const IPlaintext &compare) const override
    {
      return underlying() != dynamic_cast<const Plaintext &>(compare).underlying();
    }

  private:
    inline Plaintext(seal::Plaintext *seal_pt, bool is_owner) : underlying_(seal_pt), is_owner_(is_owner) {}

    inline seal::Plaintext &underlying() const { return *underlying_; }

    seal::Plaintext *underlying_;
    bool is_owner_;
  };
} // namespace seal_backend
} // namespace ufhe
