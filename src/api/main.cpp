#include "ufhe.hpp"
#include <iostream>

using namespace ufhe;
using namespace std;

int main()
{
  SchemeType scheme(1);
  EncryptionParameters params(scheme);
  params.set_plain_modulus(Modulus(22));
  cout << params.plain_modulus().value() << endl;
  Modulus m1(22);
  Modulus m2(33);
  Modulus m3(44);
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
