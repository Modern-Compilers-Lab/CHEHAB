#include "ufhe/ufhe.hpp"
#include <iostream>

using namespace ufhe;
using namespace std;

int main()
{
  Config::set_backend(api::backend_type::seal);
  // Parameters
  Scheme scheme(ufhe::api::scheme_type::bfv);
  EncryptionParameters params(scheme);
  size_t poly_modulus_degree = 8192;
  params.set_poly_modulus_degree(poly_modulus_degree);
  CoeffModulus coeff_modulus({8796092858369, 8796092792833, 17592186028033, 17592185438209, 17592184717313});
  params.set_coeff_modulus(coeff_modulus);
  params.set_plain_modulus(Modulus(1032193));
  cout << "plain_modulus: " << params.plain_modulus().value() << endl;
  cout << "coeff_modulus: " << endl;
  for (const ufhe::api::IModulus &e : params.coeff_modulus().value())
    cout << e.value() << " ";
  cout << endl;
  return 0;
}
