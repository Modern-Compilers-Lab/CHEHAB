#include "fhecompiler/fhecompiler.hpp"

int main()
{

  try
  {
    fhecompiler::init("matrix_mul2", 40);

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

    const int N = 81920;
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
    size_t nb_slots = 8192;
    size_t nb_lines = std::max((size_t)1, A.size() / nb_slots);
    std::vector<std::vector<fhecompiler::Ciphertext>> A_encrypted_by_column(nb_lines);
    // encrypt by column
    for (size_t j = 0; j < A[0].size(); j++)
    {
      std::vector<int64_t> batch(nb_slots, 0);
      size_t batch_index = 0;
      for (size_t i = 0; i < A.size(); i += nb_slots)
      {
        size_t l = 0;
        for (size_t k = i; k < std::min(A.size(), i + nb_slots); k++)
        {
          batch[l++] = A[k][j];
        }
        A_encrypted_by_column[batch_index++].push_back(fhecompiler::Ciphertext::encrypt(batch));
      }
    }
    std::cout << A_encrypted_by_column.size() << "\n";
    std::cout << A_encrypted_by_column[0].size() << "\n";
    std::cout << "encrypted...ok\n";
    for (size_t i = 0; i < std::max((size_t)1, A.size() / nb_slots); i++)
    {
      for (size_t j = 0; j < B[0].size(); j++)
      {
        fhecompiler::Ciphertext output("output" + std::to_string(i) + std::to_string(j), fhecompiler::VarType::output);
        output = A_encrypted_by_column[i][0] * B[0][j];
        for (size_t k = 1; k < A[0].size(); k++)
        {
          output += A_encrypted_by_column[i][k] * B[k][j];
        }
      }
    }
    std::cout << "product compueted...\n";
    // result matrix needs to be transposed after decryption

    fhecompiler::compile("matrix_mul2.hpp");
  }
  catch (const char *message)
  {
    std::cout << message << "\n";
  }

  return 0;
}
