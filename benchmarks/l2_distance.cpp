#include "fhecompiler/fhecompiler.hpp"

// BFV version

inline void set_random_vector(size_t size, std::vector<int64_t> &v)
{
  v.resize(size);
  for (size_t i = 0; i < size; i++)
    v[i] = (i & 1 ? i / 2 : 3 * i + 1);
}

inline std::vector<int64_t> make_single_value_vector(size_t size, int64_t value)
{
  std::vector<int64_t> v(size);
  for (size_t i = 0; i < size; i++)
    v[i] = value;
  return v;
}

int32_t l2_distance_clear(const std::vector<int32_t> &p1, const std::vector<int32_t> &p2)
{
  int32_t r = 0;
  if (p1.size() != p2.size())
    throw("different dimension");
  for (size_t i = 0; i < p1.size(); i++)
  {
    r += ((p1[i] - p2[i]) * (p1[i] - p2[i]));
  }
  return r;
}

int main()
{
  try
  {
    fhecompiler::init("l2_distance", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    size_t N = 8;

    fhecompiler::Ciphertext x("x", fhecompiler::VarType::input);
    std::vector<int64_t> clear_y;
    set_random_vector(N, clear_y);
    fhecompiler::Ciphertext y = fhecompiler::Ciphertext::encrypt(clear_y);

    std::vector<int64_t> m = make_single_value_vector(N, 31);
    std::vector<int64_t> b = make_single_value_vector(N, 3);

    fhecompiler::Ciphertext output("output", fhecompiler::VarType::output);
    output = y - (x * m + b);

    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    params_selector::EncryptionParameters params;
    params.set_plaintext_modulus(plaintext_modulus);
    params.set_polynomial_modulus_degree(polynomial_modulus_degree);

    fhecompiler::compile("l2_distance.hpp", &params);
  }
  catch (const char *message)
  {
    std::cerr << message << '\n';
  }

  return 0;
}
