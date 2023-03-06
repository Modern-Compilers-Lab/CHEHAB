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
    fhecompiler::init("matrix_mul_plain", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    std::vector<std::vector<int64_t>> A = {{1, 2, 3, -2}, {-5, 3, 2, 0}, {1, 0, 1, -3}, {5, 3, 2, 0}, {5, 3, 2, 0}};
    std::vector<std::vector<int64_t>> B = {{0, 1, 9}, {-7, -10, 2}, {1, 9, 0}, {-8, 2, 18}};

    const int N = 10;
    const int M = 10;
    const int P = 10;
    const int Q = 10;

    /*
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
    */

    std::vector<fhecompiler::Ciphertext> A_encrypted;
    // encrypt by line for matrix A
    for (std::vector<int64_t> line : A)
    {
      fhecompiler::Ciphertext line_encrypted = fhecompiler::Ciphertext::encrypt(line);
      A_encrypted.push_back(line_encrypted);
    }
    // transpose of B
    std::vector<std::vector<int64_t>> B_transpose;
    for (size_t column_index = 0; column_index < B[0].size(); column_index++)
    {
      std::vector<int64_t> column_data;
      for (size_t i = 0; i < B.size(); i++)
      {
        column_data.push_back(B[i][column_index]);
      }
      B_transpose.push_back(column_data);
    }

    // C contains result of multiplication
    std::vector<fhecompiler::Ciphertext> C_encrypted;
    // make outputs
    std::vector<fhecompiler::Ciphertext> outputs;

    for (size_t i = 0; i < A.size(); i++)
    {
      std::vector<fhecompiler::Ciphertext> temp_ciphers;
      for (size_t j = 0; j < B[0].size(); j++)
      {
        std::vector<int64_t> mask(A[0].size(), 0);
        mask[0] = 1;
        fhecompiler::Ciphertext simd_product = A_encrypted[i] * B_transpose[j];
        fhecompiler::Ciphertext temp_cipher = sum_all_slots(simd_product, A[0].size()) * mask;
        if (j > 0)
          temp_cipher >>= j;
        temp_ciphers.push_back(temp_cipher);
      }
      fhecompiler::Ciphertext c_line = temp_ciphers[0];
      for (size_t k = 1; k < temp_ciphers.size(); k++)
        c_line += temp_ciphers[k];

      fhecompiler::Ciphertext output("output" + std::to_string(i), fhecompiler::VarType::output);
      output = c_line;
    }

    params_selector::EncryptionParameters params;
    params.set_plaintext_modulus(plaintext_modulus);
    params.set_polynomial_modulus_degree(polynomial_modulus_degree);

    fhecompiler::compile("matrix_mul_plain.hpp", &params);
  }
  catch (const char *message)
  {
    std::cout << message << "\n";
  }

  return 0;
}