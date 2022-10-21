#pragma once

#include "encryptioncontext.hpp"
#include "galoiskeys.hpp"
#include "publickey.hpp"
#include "relinkeys.hpp"
#include "secretkey.hpp"

namespace api
{
class KeyGenerator
{
public:
  KeyGenerator(const EncryptionContext &context) { init(context); }

  KeyGenerator(const EncryptionContext &context, const SecretKey &secret_key) { init(context, secret_key); }

  virtual const SecretKey &secret_key() const = 0;

  virtual void create_public_key(PublicKey &destination) const = 0;

  virtual void create_relin_keys(RelinKeys &destination) const = 0;

  virtual void create_galois_keys(const std::vector<std::uint32_t> &galois_elts, GaloisKeys &destination) const = 0;

  virtual void create_galois_keys(const std::vector<int> &steps, GaloisKeys &destination) const = 0;

  virtual void create_galois_keys(GaloisKeys &destination) const = 0;

  // TODO: allow creating seeded objects

private:
  KeyGenerator() {}

  virtual void init(const EncryptionContext &context) = 0;

  virtual void init(const EncryptionContext &context, const SecretKey &secret_key) = 0;
};
} // namespace api
