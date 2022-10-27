#pragma once

#include "api.hpp"

namespace ufhe
{
class ISecretKey;
class IPublicKey;

class IKeyGenerator
{
public:
  virtual ~IKeyGenerator() {}

  virtual Backend backend() = 0;

  virtual const ISecretKey &secret_key() const = 0;

  virtual void create_public_key(IPublicKey &destination) const = 0;

  // virtual void create_relin_keys(RelinKeys &destination) const = 0;

  // virtual void create_galois_keys(const std::vector<std::uint32_t> &galois_elts, GaloisKeys &destination) const = 0;

  // virtual void create_galois_keys(const std::vector<int> &steps, GaloisKeys &destination) const = 0;

  // virtual void create_galois_keys(GaloisKeys &destination) const = 0;

  // TODO: allow creating seeded objects

protected:
  IKeyGenerator() = default;

  IKeyGenerator(const IKeyGenerator &copy) = default;

  IKeyGenerator &operator=(const IKeyGenerator &assign) = default;

  IKeyGenerator(IKeyGenerator &&source) = default;

  IKeyGenerator &operator=(IKeyGenerator &&assign) = default;

  // virtual void init(const IEncryptionContext &context) = 0;

  // virtual void init(const IEncryptionContext &context, const ISecretKey &secret_key) = 0;
};
} // namespace ufhe
