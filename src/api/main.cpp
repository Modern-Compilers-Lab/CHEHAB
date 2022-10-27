#include "ufhe.hpp"
#include <cstddef>
#include <iostream>

using namespace ufhe;
using namespace std;

int main()
{
  seal_backend::SchemeType scheme(1);
  seal_backend::EncryptionParameters params(scheme);
  size_t poly_modulus_degree = 8192;
  params.set_poly_modulus_degree(poly_modulus_degree);
  params.set_coeff_modulus(seal_backend::CoeffModulus::Default(poly_modulus_degree));
  params.set_plain_modulus(seal_backend::Modulus::PlainModulus(poly_modulus_degree, 20));
  cout << params.plain_modulus().value() << endl;
  for (const IModulus &e : params.coeff_modulus().value())
    cout << e.value() << " ";
  cout << endl;
  seal_backend::EncryptionContext context(params);
  seal_backend::KeyGenerator keygen(context);
  seal_backend::PublicKey pk;
  keygen.create_public_key(pk);
  return 0;
}
