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

  trs::check_ruleset(trs::Ruleset::depth_ruleset(fonc), trs::depth_order);
  clog << '\n';
  trs::check_ruleset(trs::Ruleset::ops_cost_ruleset(fonc), trs::ops_cost_order);
  clog << '\n';
  trs::check_ruleset(trs::Ruleset::joined_ruleset(fonc), trs::joined_order);

  return 0;
}
