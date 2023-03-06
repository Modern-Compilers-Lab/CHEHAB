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

inline fhecompiler::Ciphertext sum_all_slots2(const fhecompiler::Ciphertext &x, size_t vector_size)
{
  fhecompiler::Ciphertext result = x;

  auto clog2 = [](int32_t x) -> int32_t {
    int32_t r = 0;
    while (x > 1)
    {
      x /= 2;
      r += 1;
    }
    return r;
  };

  auto next_power_of_2 = [&clog2](size_t n) -> size_t {
    if (__builtin_popcount(n) == 1)
      return n;

    auto log2_of_n = clog2(n);

    // I assume no overflow as n will in range [2^10, 2^16]
    return (1 << (log2_of_n + 1));
  };

  vector_size = next_power_of_2(vector_size);
  int32_t max_num_steps = clog2(vector_size) + 1;

  int32_t rot_step = 1;
  for (; rot_step <= max_num_steps; rot_step *= 2)
  {
    fhecompiler::Ciphertext new_rotated_cipher = result << rot_step;
    result += new_rotated_cipher;
  }
  // result of sum will be in the first slot
  return result;
}

int main()
{

  try
  {
    fhecompiler::init("matrix_mul2", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    std::vector<std::vector<int64_t>> A; // = {{1, 2, 3, -2}, {-5, 3, 2, 0}, {1, 0, 1, -3}, {5, 3, 2, 0}, {5, 3, 2, 0}};
    std::vector<std::vector<int64_t>> B; // = {{0, 1, 9}, {-7, -10, 2}, {1, 9, 0}, {-8, 2, 18}};

    /*

      Matrix A is encrypted matrix and B is plaintext matrix (matrix of scalars)


    {1, 2, 3, -2}
    {-5, 3, 2, 0}
    {1, 0, 1, -3}
    {5, 3, 2, 0}
    {5, 3, 2, 0}

    {0, 1, 9}
    {-7, -10, 2}
    {1, 9, 0}
    {-8, 2, 18}



    4x3

    {1, -5, 1, 5, 5} 0
    {2, 3, 0, 3, 3} -7
    {3, 2, 1, 2, 2} 1
    {-2, 0, -3, 0, 0} -8

    {0}

    4x5

    */

    const int N = 10;
    const int M = 10;
    const int P = 10;
    const int Q = 10;

    for (size_t i = 0; i < N; i++)
    {
      std::vector<int64_t> line;
      for (size_t j = 0; j < M; j++)
      {
        line.push_back((i + 1) * (j + 1));
      }
      A.push_back(line);
    }
    for (size_t i = 0; i < P; i++)
    {
      std::vector<int64_t> line;
      for (size_t j = 0; j < Q; j++)
      {
        line.push_back((i + 1) * (j + 1));
      }
      B.push_back(line);
    }

    std::vector<fhecompiler::Ciphertext> A_encrypted_by_column;
    // encrypt by column
    for (size_t j = 0; j < A[0].size(); j++)
    {
      std::vector<int64_t> column(A.size());
      for (size_t i = 0; i < A.size(); i++)
      {
        column[i] = A[i][j];
      }
      A_encrypted_by_column.push_back(fhecompiler::Ciphertext::encrypt(column));
    }
    // B stays as it is, a matrix of scalars
    for (size_t j = 0; j < B[0].size(); j++)
    {
      // for each column of scalars matrix we computed a row of result matrix
      std::vector<fhecompiler::Ciphertext> tmp_ciphers;
      for (size_t i = 0; i < B.size(); i++)
      {
        fhecompiler::Ciphertext tmp_cipher = A_encrypted_by_column[i] * B[i][j];
        tmp_ciphers.push_back(tmp_cipher);
      }
      fhecompiler::Ciphertext output("output" + std::to_string(j), fhecompiler::VarType::output);
      output = tmp_ciphers[0];
      for (size_t i = 1; i < B.size(); i++)
        output += tmp_ciphers[i];
    }
    // result matrix needs to be transposed after decryption
    params_selector::EncryptionParameters params;
    params.set_plaintext_modulus(plaintext_modulus);
    params.set_polynomial_modulus_degree(polynomial_modulus_degree);

    fhecompiler::compile("matrix_mul2.hpp", &params);
  }
  catch (const char *message)
  {
    std::cout << message << "\n";
  }

  return 0;
}
