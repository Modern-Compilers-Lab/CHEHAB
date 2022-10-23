#include "seal_backend/seal_backend.hpp"
#include "ufhe.hpp"
#include <iostream>

using namespace ufhe;
using namespace std;

int main()
{
  auto ptr = Modulus::create(Backend::seal, 22);
  cout << ptr->value() << endl;
  Modulus &modulus = *ptr;
  cout << typeid(modulus).name() << endl;
  return 0;
}
