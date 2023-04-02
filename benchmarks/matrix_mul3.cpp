#include "fhecompiler/fhecompiler.hpp"

int main()
{

  try
  {
    fhecompiler::init("matrix_mul3", 40);

    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    std::vector<std::vector<int64_t>> A; // = {{1, 2, 3, -2}, {-5, 3, 2, 0}, {1, 0, 1, -3}, {5, 3, 2, 0}, {5, 3, 2, 0}};
    std::vector<std::vector<int64_t>> B; // = {{0, 1, 9}, {-7, -10, 2}, {1, 9, 0}, {-8, 2, 18}};

    const int N = 10;
    const int M = 64;
    const int P = 64;
    const int Q = 256;

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
        line.push_back((j + 1) * (i + 1));
      }
      B.push_back(line);
    }

    std::vector<std::vector<fhecompiler::Ciphertext>> A_encrypted(
      A.size(), std::vector<fhecompiler::Ciphertext>(A[0].size()));
    for (size_t i = 0; i < A.size(); i++)
    {
      for (size_t j = 0; j < A[0].size(); j++)
      {
        fhecompiler::Ciphertext input("input" + std::to_string(i) + std::to_string(j), fhecompiler::VarType::input);
        std::vector<int64_t> ee = {A[i][j]};
        A_encrypted[i][j] = input; // fhecompiler::Ciphertext::encrypt(ee);
      }
    }
    // standard matrix multiplication
    for (size_t i = 0; i < A.size(); i++)
    {
      for (size_t j = 0; j < B[0].size(); j++)
      {
        // compute entry ij
        fhecompiler::Ciphertext output("output" + std::to_string(i) + std::to_string(j), fhecompiler::VarType::output);
        output = A_encrypted[i][0] * B[0][j];
        for (size_t k = 1; k < A[0].size(); k++)
        {
          output += A_encrypted[i][k] * B[k][j];
        }
      }
    }
    // result matrix needs to be transposed after decryption

    fhecompiler::compile("matrix_mul3.hpp");
  }
  catch (const char *message)
  {
    std::cout << message << "\n";
  }

  return 0;
}
