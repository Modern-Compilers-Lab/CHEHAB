#include<bits/stdc++.h>
#include"fhecompiler.hpp"

using namespace std;

using namespace fhecompiler;

Ciphertext sum(const Ciphertext& a, const Ciphertext& b)
{
  return a+b;
}

void print_string(std::optional<std::string> string_opt)
{
  if(string_opt != nullopt) std::cout << *string_opt << "\n";
}

int main()
{

  fhecompiler::init("test1", 1024);

  fhecompiler::set_program_scheme(fhecompiler::bfv);

  fhecompiler::Plaintext pt1(std::vector<int64_t>{1,3,4,5});
  pt1.set_as_output("z_plain");
  fhecompiler::Ciphertext ct1(pt1);

  fhecompiler::Ciphertext ct2 = ct1+ct1;

  ct2.set_as_output("z");

  fhecompiler::Ciphertext ct3 = ct2;
  ct3.set_as_output("ct3");
  ct3 += ct1;

  fhecompiler::Scalar sc1 = 2;
  fhecompiler::Scalar sc2 = 3;
  fhecompiler::Scalar sc3 = sc1+sc2;
  sc3 += sc1;
  sc3 = -sc3;

  fhecompiler::compile();

  return 0;
  
}
