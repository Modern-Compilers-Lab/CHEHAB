#include "fhecompiler.hpp"
#include <bits/stdc++.h>

using namespace std;

using namespace fhecompiler;

Ciphertext sum(const Ciphertext &a, const Ciphertext &b)
{
  return a + b;
}

void print_string(std::optional<std::string> string_opt)
{
  if (string_opt != nullopt)
    std::cout << *string_opt << "\n";
}

int main()
{

  try
  {

    fhecompiler::init("test1", 1024, fhecompiler::bfv);

    // scheme better to be introduced in init

    fhecompiler::Plaintext pt1(std::vector<int64_t>{1, 3, 4, 5});

    fhecompiler::Plaintext pt2("pt2_input", VarType::input);

    fhecompiler::Plaintext pt3("pt3"); //temp is default type

    pt1 *= pt1;

    pt3 = pt1*123;

    fhecompiler::Ciphertext ct1 = fhecompiler::Ciphertext::encrypt(pt1);

    fhecompiler::Ciphertext ct2 = ct1 + ct1;

    fhecompiler::Ciphertext ct3 = ct2;
    ct3 += ct1;

    ct3 += ct3;

    fhecompiler::Scalar sc1 = 2;
    fhecompiler::Scalar sc2 = 3;
    fhecompiler::Scalar sc3 = sc1 + sc2;
    sc3 += sc1;
    sc3 = -sc3;

    fhecompiler::Ciphertext ct4 = ct3 + pt1;

    fhecompiler::Ciphertext ct7 = ct3 - pt1;

    Ciphertext ct5("x", VarType::output);
    ct5 = ct4;
    // ct5 += ct4;
    // ct5 += ct5;
    Ciphertext ct6 = 123 * ct1;
    ct6 -= 12;

    fhecompiler::compile();
  }
  catch (const char *message)
  {
    std::cout << message << "\n";
  }

  return 0;
}