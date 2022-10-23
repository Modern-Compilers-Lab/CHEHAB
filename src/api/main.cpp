#include "api_includes.hpp"
#include "seal_backend/seal_backend.hpp"
#include <iostream>

using namespace std;

int main()
{
  auto ptr = api::Modulus::create(api::Backend::seal, 22);
  cout << ptr->value() << endl;
  api::Modulus &modulus = *ptr;
  return 0;
}
