#include "fheco/fheco.hpp"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using namespace fheco;

int main(int argc, char **argv)
{
  string nom_app = "exemple_present";
  size_t nb_pos = 4;
  int l_bits = 20;
  bool arith_signe = true;
  bool rotation_cycl = false;

  const auto &fonc = Compiler::create_func(nom_app, nb_pos, l_bits, arith_signe, rotation_cycl);

  Ciphertext a("a");
  Ciphertext b("b");
  Ciphertext c("c");
  Ciphertext resultat = a * (b - c) + a * (0 + c);
  resultat.set_output("résultat");

  ofstream flux_ri(nom_app + "_noopt_ir_nocodegen.dot");
  util::draw_ir(fonc, flux_ri, true);

  return 0;
}
