#pragma once

#include "interface.hpp"
#include "ipublickey.hpp"
#include "isecretkey.hpp"

namespace ufhe
{
class IKeyGenerator : public virtual Interface
{
public:
  virtual const ISecretKey &secret_key() const = 0;

  virtual void create_public_key(IPublicKey &destination) const = 0;

  // virtual void create_relin_keys(RelinKeys &destination) const = 0;

  // virtual void create_galois_keys(const std::vector<std::uint32_t> &galois_elts, GaloisKeys &destination) const = 0;

  // virtual void create_galois_keys(const std::vector<int> &steps, GaloisKeys &destination) const = 0;

  // virtual void create_galois_keys(GaloisKeys &destination) const = 0;

  // TODO: allow creating seeded objects
};
} // namespace ufhe
