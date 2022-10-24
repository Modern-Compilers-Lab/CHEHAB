#include "seal_backend/seal_backend.hpp"
#include "ufhe.hpp"
#include <iostream>

using namespace ufhe;
using namespace std;

int main()
{
  Modulus::ptr modulus = Modulus::create(Backend::seal, 22);
  cout << modulus->value() << endl;
  cout << typeid(modulus).name() << endl;
  SchemeType::ptr scheme = SchemeType::create(1);
  EncryptionParameters::ptr params = EncryptionParameters::create(*scheme);
  return 0;
}
