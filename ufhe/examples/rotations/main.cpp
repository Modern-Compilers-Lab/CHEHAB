#include "ufhe/ufhe.hpp"
#include <iomanip>
#include <iostream>

using namespace ufhe;
using namespace std;

/*
Helper function: Prints a matrix of values.
*/
template <typename T>
inline void print_matrix(std::vector<T> matrix, std::size_t row_size)
{
  /*
  We're not going to print every column of the matrix (there are 2048).
  Instead print this many slots from beginning and end of the matrix.
  */
  std::size_t print_size = 5;

  std::cout << std::endl;
  std::cout << "    [";
  for (std::size_t i = 0; i < print_size; i++)
  {
    std::cout << std::setw(3) << std::right << matrix[i] << ",";
  }
  std::cout << std::setw(3) << " ...,";
  for (std::size_t i = row_size - print_size; i < row_size; i++)
  {
    std::cout << std::setw(3) << matrix[i] << ((i != row_size - 1) ? "," : " ]\n");
  }
  std::cout << "    [";
  for (std::size_t i = row_size; i < row_size + print_size; i++)
  {
    std::cout << std::setw(3) << matrix[i] << ",";
  }
  std::cout << std::setw(3) << " ...,";
  for (std::size_t i = 2 * row_size - print_size; i < 2 * row_size; i++)
  {
    std::cout << std::setw(3) << matrix[i] << ((i != 2 * row_size - 1) ? "," : " ]\n");
  }
  std::cout << std::endl;
}

int main()
{
  Config::set_backend(api::backend_type::seal);
  // Parameters
  Scheme scheme(api::scheme_type::bfv);
  EncryptionParams params(scheme);
  size_t poly_modulus_degree = 8192;
  params.set_poly_modulus_degree(poly_modulus_degree);
  CoeffModulus coeff_modulus({8796092858369, 8796092792833, 17592186028033, 17592185438209, 17592184717313});
  params.set_coeff_modulus(coeff_modulus);
  params.set_plain_modulus(Modulus(1032193));
  cout << "plain_modulus: " << params.plain_modulus().value() << endl;
  cout << "coeff_modulus: " << endl;
  for (const ufhe::api::Modulus &e : params.coeff_modulus().value())
    cout << e.value() << " ";
  cout << endl;
  EncryptionContext context(params);

  // Input preparation
  BatchEncoder batch_encoder(context);
  size_t slot_count = batch_encoder.slot_count();
  cout << "slot_count " << slot_count << endl;
  vector<int64_t> a_clear(slot_count);
  for (int i = 0; i < slot_count; i++)
    a_clear[i] = i;
  vector<int64_t> b_clear(slot_count);
  for (int i = 0; i < slot_count; i++)
    b_clear[i] = i;
  print_matrix(a_clear, slot_count / 2);

  // Encode
  Plaintext a_plain;
  batch_encoder.encode(a_clear, a_plain);
  Plaintext b_plain;
  batch_encoder.encode(b_clear, b_plain);

  // Encrypt
  KeyGenerator keygen(context);
  PublicKey pk;
  keygen.create_public_key(pk);
  Encryptor encryptor(context, pk);
  Ciphertext a_encrypted;
  encryptor.encrypt(a_plain, a_encrypted);
  Ciphertext b_encrypted;
  encryptor.encrypt(b_plain, b_encrypted);

  // Evaluate
  GaloisKeys gk;
  keygen.create_galois_keys(gk);
  Evaluator evaluator(context);
  evaluator.rotate_inplace(a_encrypted, 2, gk);
  evaluator.rotate(b_encrypted, 2, gk, b_encrypted);
  //   Decrypt
  const SecretKey &sk = keygen.secret_key();
  Decryptor decryptor(context, sk);
  decryptor.decrypt(a_encrypted, a_plain);
  decryptor.decrypt(b_encrypted, b_plain);
  //  Decode
  batch_encoder.decode(a_plain, a_clear);
  batch_encoder.decode(b_plain, b_clear);
  //  Show results
  print_matrix(a_clear, slot_count / 2);
  print_matrix(b_clear, slot_count / 2);
  return 0;
}
