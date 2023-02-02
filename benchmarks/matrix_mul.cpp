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
    fhecompiler::init("matrix_mul", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    std::vector<std::vector<int64_t>> A = {{1, 2, 3, -2}, {-5, 3, 2, 0}, {1, 0, 1, -3}, {5, 3, 2, 0}, {5, 3, 2, 0}};
    std::vector<std::vector<int64_t>> B = {{0, 1, 9}, {-7, -10, 2}, {1, 9, 0}, {-8, 2, 18}};

    /*
    size_t N = 10;
    size_t M = 10;
    for (size_t i = 0; i < N; i++)
    {
      std::vector<int64_t> line;
      for (size_t j = 0; j < M; j++)
      {
        line.push_back((i + 1) * (j + 1));
      }
      A.push_back(line);
    }

    for (size_t i = 0; i < N; i++)
    {
      std::vector<int64_t> line;
      for (size_t j = 0; j < M; j++)
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
    // encrypt by column for matrix B
    std::vector<fhecompiler::Ciphertext> B_encrypted;
    for (size_t column_index = 0; column_index < B[0].size(); column_index++)
    {
      std::vector<int64_t> column_data;
      for (size_t i = 0; i < B.size(); i++)
      {
        column_data.push_back(B[i][column_index]);
      }
      fhecompiler::Ciphertext column_encrypted = fhecompiler::Ciphertext::encrypt(column_data);
      B_encrypted.push_back(column_encrypted);
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
        fhecompiler::Ciphertext simd_product = A_encrypted[i] * B_encrypted[j];
        fhecompiler::Ciphertext temp_cipher = sum_all_slots(simd_product, A[0].size()) * mask;
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

    fhecompiler::compile("matrix_mul.hpp", &params);
  }
  catch (const char *message)
  {
    std::cout << message << "\n";
  }

  return 0;
}
