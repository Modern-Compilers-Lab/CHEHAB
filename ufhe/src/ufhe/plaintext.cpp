#include "ufhe/plaintext.hpp"
#include "ufhe/seal_backend/plaintext.hpp"

namespace ufhe
{
Plaintext::Plaintext(api::backend_type backend)
{
  switch (backend)
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::Plaintext>();
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

Plaintext::Plaintext(api::backend_type backend, std::size_t coeff_count)
{
  switch (backend)
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::Plaintext>(coeff_count);
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

Plaintext::Plaintext(std::size_t coeff_count) : Plaintext(Config::backend(), coeff_count) {}

Plaintext::Plaintext(api::backend_type backend, const std::string &hex_poly)
{
  switch (backend)
  {
  case api::backend_type::seal:
    underlying_ = std::make_shared<seal_backend::Plaintext>(hex_poly);
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

Plaintext::Plaintext(const std::string &hex_poly) : Plaintext(Config::backend(), hex_poly) {}

Plaintext::Plaintext(const Plaintext &copy)
{
  switch (copy.backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::Plaintext>(static_cast<const seal_backend::Plaintext &>(copy.underlying()));
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

Plaintext &Plaintext::operator=(const Plaintext &assign)
{

  switch (assign.backend())
  {
  case api::backend_type::seal:
    underlying_ =
      std::make_shared<seal_backend::Plaintext>(static_cast<const seal_backend::Plaintext &>(assign.underlying()));
    return *this;
    break;

  case api::backend_type::none:
    throw std::invalid_argument("no backend is selected");
    break;

  default:
    throw std::invalid_argument("unsupported backend");
    break;
  }
}

void Plaintext::resize(std::size_t coeff_count)
{
  underlying_->resize(coeff_count);
}

void Plaintext::set_zero(std::size_t start_coeff, std::size_t length)
{
  underlying_->set_zero(start_coeff, length);
}

std::size_t Plaintext::capacity() const
{
  return underlying().capacity();
}

std::size_t Plaintext::coeff_count() const
{
  return underlying().coeff_count();
}

std::string Plaintext::to_string() const
{
  return underlying().to_string();
}

bool Plaintext::operator==(const api::Plaintext &compare) const
{
  return underlying() == safe_static_cast<const Plaintext &>(compare).underlying();
}

bool Plaintext::operator!=(const api::Plaintext &compare) const
{
  return underlying() != safe_static_cast<const Plaintext &>(compare).underlying();
}
} // namespace ufhe
