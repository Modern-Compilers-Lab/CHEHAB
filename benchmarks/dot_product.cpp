#include "fhecompiler/fhecompiler.hpp"

inline fhecompiler::Ciphertext sum_all_slots(const fhecompiler::Ciphertext &x, int vector_size)
{
  std::vector<fhecompiler::Ciphertext> rotated_ciphers = {x};
  fhecompiler::Ciphertext result = rotated_ciphers.back();
  for (; vector_size > 0; vector_size--)
  {
    fhecompiler::Ciphertext cipher_rotated = rotated_ciphers.back() << 1;
    result += cipher_rotated;
    rotated_ciphers.push_back(cipher_rotated);
  }
  // result of sum will be in the first slot
  return result;
}

int main()
{

  try
  {
    fhecompiler::init("dot_product", 40);

    fhecompiler::Ciphertext A("A", fhecompiler::VarType::input);
    fhecompiler::Ciphertext B("B", fhecompiler::VarType::input);

    size_t vector_size = 1024;

    fhecompiler::Ciphertext simd_product = A * B;

    fhecompiler::Ciphertext C("C", fhecompiler::VarType::output);
    C = sum_all_slots(simd_product, vector_size); // Result of the dot product is stored in the first slot

    fhecompiler::compile("dot_product.hpp");
  }
  catch (const char *message)
  {
    std::cerr << message << '\n';
  }

  return 0;
}
