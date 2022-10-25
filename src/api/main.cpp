#include "ufhe.hpp"
#include <iostream>

using namespace ufhe;
using namespace std;

int main()
{
  Modulus::ptr modulus = Modulus::create(Backend::seal, 22);
  cout << modulus->value() << endl;
  SchemeType::ptr scheme = SchemeType::create(1);
  EncryptionParameters::ptr params = EncryptionParameters::create(scheme);
  const SchemeType &scheme_ref = params->scheme();
  cout << typeid(scheme_ref).name() << endl;
  return 0;
}
