#include "fheco/fheco.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using namespace fheco;

void box_blur(size_t height, size_t width)
{
  vector<size_t> shape{height, width};
  const Ciphertext img("img", shape);
  Ciphertext top_row = img >> width;
  Ciphertext bottom_row = img << width;
  Ciphertext top_sum = (top_row >> 1) + top_row + (top_row << 1);
  Ciphertext curr_sum = (img >> 1) + img + (img << 1);
  Ciphertext bottom_sum = (bottom_row >> 1) + bottom_row + (bottom_row << 1);
  Ciphertext result = top_sum + curr_sum + bottom_sum;
  result.set_output("result");

  Ciphertext result1 = Plaintext(0, shape);
  for (size_t i = 1; i < height - 1; ++i)
  {
    for (size_t j = 1; j < width - 1; ++j)
    {
      result1[i][j] = img[i - 1][j + 1] + // Top left
                      img[i + 0][j + 1] + // Top center
                      img[i + 1][j + 1] + // Top right
                      img[i - 1][j + 0] + // Mid left
                      img[i + 0][j + 0] + // Current pixel
                      img[i + 1][j + 0] + // Mid right
                      img[i - 1][j - 1] + // Low left
                      img[i + 0][j - 1] + // Low center
                      img[i + 1][j - 1]; // Low right
    }
  }
  result1.set_output("result1");
}

int main(int argc, char **argv)
{
  size_t width = 64;
  if (argc > 1)
    width = stoul(argv[1]);

  size_t height = 64;
  if (argc > 2)
    height = stoul(argv[2]);

  int trs_passes = 1;
  if (argc > 3)
    trs_passes = stoi(argv[3]);

  bool optimize = trs_passes > 0;

  cout << "width: " << width << ", "
       << "height: " << height << ", "
       << "trs_passes: " << trs_passes << '\n';

  string func_name = "box_blur";
  Compiler::create_func(func_name, height * width, 16, true);
  box_blur(height, width);

  // ofstream init_ir_os(func_name + "_init_ir.dot");
  // util::draw_ir(*Compiler::active_func(), init_ir_os);

  // util::Quantifier quantifier(Compiler::active_func());
  // quantifier.run_analysis();
  // quantifier.print_info(cout);

  // util::ExprPrinter expr_printer(Compiler::active_func());
  // expr_printer.compute_terms_str_expr(util::ExprPrinter::Mode::infix_explicit_parenthesis);
  // expr_printer.print_outputs_str_expr(cout);
  // cout << Compiler::active_func()->constants_values();

  // const auto &rand_inputs = Compiler::active_func()->inputs_info();
  // auto outputs = util::evaluate_on_clear(*Compiler::active_func(), rand_inputs);
  // if (outputs != Compiler::active_func()->outputs_info())
  //   throw logic_error("compilation correctness-test failed");

  // ofstream rand_example_os(func_name + "_rand_example.txt");
  // util::print_io_terms_values(*Compiler::active_func(), rand_example_os);
  return 0;
}
