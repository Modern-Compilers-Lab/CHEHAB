#include "ufhe.hpp"
#include <iostream>

using namespace ufhe;
using namespace std;

int main()
{
  seal_backend::SchemeType scheme(1);
  seal_backend::EncryptionParameters params(scheme);
  params.set_plain_modulus(seal_backend::Modulus(22));
  cout << params.plain_modulus().value() << endl;
  seal_backend::Modulus m1(22);
  seal_backend::Modulus m2(33);
  seal_backend::Modulus m3(44);
  IModulus::vector v;
  v.push_back(m1);
  v.push_back(m2);
  v.push_back(m3);
  params.set_coeff_modulus(v);
  for (const IModulus &e : params.coeff_modulus())
    cout << e.value() << " ";
  cout << endl;
  return 0;
}
