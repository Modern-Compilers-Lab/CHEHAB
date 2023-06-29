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

  Ciphertext a("a", -10, 10);
  Ciphertext b("b", -10, 10);
  Ciphertext c("c", -10, 10);
  Ciphertext resultat = a * (b - c) + a * (0 + c);
  resultat.set_output("résultat");

  string nom_gen = "gen_he_" + nom_app;
  string chemin_gen = "he/" + nom_gen;
  ofstream flux_entete(chemin_gen + ".hpp");
  ofstream flux_source(chemin_gen + ".cpp");
  Compiler::gen_he_code(fonc, flux_entete, nom_gen + ".hpp", flux_source);

  ofstream flux_exemple_es(nom_app + "_io_example.txt");
  util::print_io_terms_values(fonc, flux_exemple_es);

  ofstream flux_ri(nom_app + "_noopt_ir.dot");
  util::draw_ir(fonc, flux_ri, true);

  return 0;
}
