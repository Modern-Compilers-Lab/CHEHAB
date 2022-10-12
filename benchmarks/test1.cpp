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
  /*
  Scalar sc1 = 12;
  std::cout << sc1.get_label() << "\n"; //object0
  Scalar sc2 = 13;
  std::cout << sc2.get_label() << "\n; //object1
  Scalar sc3("y",true, false);
  std::cout << sc3.get_label() << "\n"; //object2
  sc3 = sc1+sc2;
  std::cout << sc3.get_label() << "\n";
  Scalar sc4 = sc3+sc1;
  sc3 = sc1-sc2;
  std::cout << sc3.get_label() << "\n";
  Scalar sc5 = sc3+1;
  
  Scalar x("x", false, false);
  std::cout << x.get_label() << "\n";
  Scalar y = (Scalar )2+3;
  */
  /*
  Plaintext pt1 = std::vector<int64_t>{23, 32};
  Plaintext pt2 = std::vector<int>{23,11};
  
  Plaintext pt3("y", true, false);
  pt3 = pt1+pt2;
  pt3 = pt2+pt1;
  Plaintext pt4 = pt3+pt1;
  */
  /*
  Scalar pt1 = 34;
  Scalar pt2 = 11;
  Scalar pt3 = pt1 + pt2;
  */

  fhecompiler::init("test1");
  fhecompiler::set_program_scheme(fhecompiler::bfv);
  Plaintext pt1(std::vector<int64_t>{1,3,4,5});

  Ciphertext ct1(pt1); //
  Ciphertext ct2 = ct1;
  Ciphertext ct3 = ct1 + ct2 * ct1;
  Ciphertext ct4 = ct3+ct3;

  Scalar sc1 = 12+34;
  Scalar sc2 = sc1-5*sc1;

  ct3.set_as_output("y");

  Ciphertext ct5 = ct3+ct1;
  return 0;
}
