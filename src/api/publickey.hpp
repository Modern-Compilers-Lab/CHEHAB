#pragma once

namespace api
{
class PublicKey
{
public:
  PublicKey() { init(); }

  virtual ~PublicKey() {}

  // TODO: Allow access to the key parms_id

  // TODO: Serialization support
private:
  virtual void init() = 0;
};
} // namespace api
