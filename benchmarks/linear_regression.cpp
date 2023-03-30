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

int main()
{
  try
  {
    fhecompiler::init("linear_regression", 40);

    size_t N = 8;

    fhecompiler::Ciphertext x("x", fhecompiler::VarType::input);
    std::vector<int64_t> clear_y;
    set_random_vector(N, clear_y);
    fhecompiler::Ciphertext y = fhecompiler::Ciphertext::encrypt(clear_y);

    std::vector<int64_t> m = make_single_value_vector(N, 31);
    std::vector<int64_t> b = make_single_value_vector(N, 3);

    fhecompiler::Ciphertext output("output", fhecompiler::VarType::output);
    output = y - (x * m + b);

    fhecompiler::compile("linear_regression.hpp");
  }
  catch (const char *message)
  {
    std::cerr << message << '\n';
  }

  return 0;
}
