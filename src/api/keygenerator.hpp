#pragma once

#include "secretkey.hpp"

namespace api {
class KeyGenerator {
    virtual const SecretKey &secret_key() const = 0;

    virtual void create_public_key(PublicKey &destination) const = 0;
};
}  // namespace api