#include "ufhe/seal_backend/encryption_context.hpp"
#include "seal/context.h"
#include "ufhe/seal_backend/encryption_params.hpp"

namespace ufhe
{
namespace seal_backend
{
  EncryptionContext::EncryptionContext(const EncryptionParams &params)
    : underlying_(std::make_shared<seal::SEALContext>(params.underlying()))
  {}
} // namespace seal_backend
} // namespace ufhe
