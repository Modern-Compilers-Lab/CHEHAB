#pragma once

#include "ufhe/api/abstract_type.hpp"

namespace ufhe
{
namespace api
{
  class PublicKey;
  class GaloisKeys;
  class RelinKeys;
  class SecretKey;

  class KeyGenerator : public AbstractType
  {
  public:
    virtual const SecretKey &secret_key() const = 0;

    virtual void create_public_key(PublicKey &destination) const = 0;

    virtual void create_relin_keys(RelinKeys &destination) const = 0;

    // virtual void create_galois_keys(const std::vector<std::uint32_t> &galois_elts, GaloisKeys &destination) const =
    // 0;

    // virtual void create_galois_keys(const std::vector<int> &steps, GaloisKeys &destination) const = 0;

    virtual void create_galois_keys(GaloisKeys &destination) const = 0;

    // TODO: allow creating seeded objects
  };
} // namespace api
} // namespace ufhe
