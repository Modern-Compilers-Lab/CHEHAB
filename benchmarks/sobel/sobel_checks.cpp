#include "fheco/fheco.hpp"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using namespace fheco;

void sobel(size_t width)
{
  Ciphertext img("img", 0, 255);
  Ciphertext top_row = img >> width;
  Ciphertext bottom_row = img << width;
  // gx
  vector<vector<integer>> gx_kernel = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
  Ciphertext gx_top_sum =
    gx_kernel[0][0] * (top_row >> 1) + gx_kernel[0][1] * top_row + gx_kernel[0][2] * (top_row << 1);
  Ciphertext gx_curr_sum = gx_kernel[1][0] * (img >> 1) + gx_kernel[1][1] * img + gx_kernel[1][2] * (img << 1);
  Ciphertext gx_bottom_sum =
    gx_kernel[2][0] * (bottom_row >> 1) + gx_kernel[2][1] * bottom_row + gx_kernel[2][2] * (bottom_row << 1);
  Ciphertext gx_result = gx_top_sum + gx_curr_sum + gx_bottom_sum;
  // gy
  vector<vector<integer>> gy_kernel = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
  Ciphertext gy_top_sum =
    gy_kernel[0][0] * (top_row >> 1) + gy_kernel[0][1] * top_row + gy_kernel[0][2] * (top_row << 1);
  Ciphertext gy_curr_sum = gy_kernel[1][0] * (img >> 1) + gy_kernel[1][1] * img + gy_kernel[1][2] * (img << 1);
  Ciphertext gy_bottom_sum =
    gy_kernel[2][0] * (bottom_row >> 1) + gy_kernel[2][1] * bottom_row + gy_kernel[2][2] * (bottom_row << 1);
  Ciphertext gy_result = gy_top_sum + gy_curr_sum + gy_bottom_sum;
  // combine
  Ciphertext result = gx_result * gx_result + gy_result * gy_result;
  result.set_output("result");
}

void sobel_baseline(size_t width)
{
  Ciphertext img("img", 0, 255);
  // gx
  Ciphertext gx_top_sum = -(img >> (width + 1)) + (img >> (width - 1));
  Ciphertext gx_curr_sum = -2 * (img >> 1) + 2 * (img << 1);
  Ciphertext gx_bottom_sum = -(img << (width - 1)) + (img << (width + 1));
  Ciphertext gx_result = gx_top_sum + gx_curr_sum + gx_bottom_sum;
  // gy
  Ciphertext gy_top_sum = -(img >> (width + 1)) - 2 * (img >> width) - (img >> (width - 1));
  Ciphertext gy_bottom_sum = (img << (width - 1)) + 2 * (img << width) + (img << (width + 1));
  Ciphertext gy_result = gy_top_sum + gy_bottom_sum;
  // combine
  Ciphertext result = square(gx_result) + square(gy_result);
  result.set_output("result");
}

void print_bool_arg(bool arg, const string &name, ostream &os)
{
  os << (arg ? name : "no_" + name);
}

int main(int argc, char **argv)
{
  bool call_quantifier = false;
  if (argc > 1)
    call_quantifier = stoi(argv[1]);

  auto ruleset = Compiler::Ruleset::joined;
  if (argc > 2)
    ruleset = static_cast<Compiler::Ruleset>(stoi(argv[2]));

  auto rewrite_heuristic = trs::RewriteHeuristic::bottom_up;
  if (argc > 3)
    rewrite_heuristic = static_cast<trs::RewriteHeuristic>(stoi(argv[3]));

  bool cse = true;
  if (argc > 4)
    cse = stoi(argv[4]);

  bool const_folding = true;
  if (argc > 5)
    const_folding = stoi(argv[5]);

  print_bool_arg(call_quantifier, "quantifier", clog);
  clog << " ";
  clog << ruleset << "_trs";
  clog << " ";
  clog << rewrite_heuristic;
  clog << " ";
  print_bool_arg(cse, "cse", clog);
  clog << " ";
  print_bool_arg(const_folding, "constant_folding", clog);
  clog << '\n';

  string app_name = "sobel";
  size_t width = 64;
  size_t height = 64;
  size_t slot_count = width * height;
  int bit_width = 20;
  bool signdness = true;
  bool need_cyclic_rotation = true;

  clog << "\nnoopt function\n";
  string noopt_func_name = app_name + "_noopt";
  const auto &noopt_func =
    Compiler::create_func(noopt_func_name, slot_count, bit_width, signdness, need_cyclic_rotation);
  sobel(width);

  string noopt_gen_name = "gen_he_" + noopt_func_name;
  string noopt_gen_path = "he/" + noopt_gen_name;
  ofstream noopt_header_os(noopt_gen_path + ".hpp");
  if (!noopt_header_os)
    throw logic_error("failed to create noopt_header file");

  ofstream noopt_source_os(noopt_gen_path + ".cpp");
  if (!noopt_source_os)
    throw logic_error("failed to create noopt_source file");

  Compiler::gen_he_code(noopt_func, noopt_header_os, noopt_gen_name + ".hpp", noopt_source_os);

  ofstream noopt_ir_os(noopt_func_name + "_ir.dot");
  if (!noopt_ir_os)
    throw logic_error("failed to create noopt_ir file");

  util::draw_ir(noopt_func, noopt_ir_os);
  util::Quantifier noopt_quantifier(noopt_func);
  if (call_quantifier)
  {
    cout << "\ninitial circuit characteristics\n";
    noopt_quantifier.run_all_analysis();
    noopt_quantifier.print_info(cout);
    cout << endl;
  }

  clog << "\nopt function\n";
  if (cse)
  {
    Compiler::enable_cse();
    Compiler::enable_order_operands();
  }
  else
  {
    Compiler::disable_cse();
    Compiler::disable_order_operands();
  }

  if (const_folding)
    Compiler::enable_const_folding();
  else
    Compiler::disable_const_folding();

  string opt_func_name = app_name + "_opt";
  const auto &opt_func = Compiler::create_func(opt_func_name, slot_count, bit_width, signdness, need_cyclic_rotation);
  sobel(width);

  string opt_gen_name = "gen_he_" + opt_func_name;
  string opt_gen_path = "he/" + opt_gen_name;
  ofstream opt_header_os(opt_gen_path + ".hpp");
  if (!opt_header_os)
    throw logic_error("failed to create opt_header file");

  ofstream opt_source_os(opt_gen_path + ".cpp");
  if (!opt_source_os)
    throw logic_error("failed to create opt_source file");

  Compiler::compile(opt_func, ruleset, rewrite_heuristic, opt_header_os, opt_gen_name + ".hpp", opt_source_os);

  auto noopt_obtained_outputs = util::evaluate_on_clear(noopt_func, opt_func->get_inputs_example_values());
  auto opt_obtained_outputs = util::evaluate_on_clear(opt_func, noopt_func->get_inputs_example_values());
  if (
    noopt_obtained_outputs != opt_func->get_outputs_example_values() ||
    opt_obtained_outputs != noopt_func->get_outputs_example_values())
    throw logic_error("compilation correctness-test failed");

  ofstream io_example_os(app_name + "_io_example.txt");
  if (!io_example_os)
    throw logic_error("failed to create io_example file");

  util::print_io_terms_values(noopt_func, io_example_os);
  ofstream opt_ir_os(opt_func_name + "_ir.dot");
  if (!opt_ir_os)
    throw logic_error("failed to create opt_ir file");

  util::draw_ir(opt_func, opt_ir_os);
  if (call_quantifier)
  {
    cout << "\nfinal circuit characteristics\n";
    util::Quantifier opt_quantifier(opt_func);
    opt_quantifier.run_all_analysis();
    opt_quantifier.print_info(cout);

    cout << "\nimprovment rates\n";
    auto diff_quantifier = (noopt_quantifier - opt_quantifier) / noopt_quantifier * 100;
    diff_quantifier.print_info(cout);
  }
  return 0;
}
