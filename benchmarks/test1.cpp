#include<bits/stdc++.h>
#include"fhecompiler.hpp"

using namespace std;

int main()
{
  /*
  Scalar sc1 = 12;
  std::cout << sc1.get_label() << "\n"; //object0
  Scalar sc2 = 13;
  std::cout << sc2.get_label() << "\n"; //object1
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

  fhecompiler::Plaintext pt1 = std::vector<int64_t>{1,2,3,4};
  std::cout << pt1.get_label() << "\n";

  fhecompiler::Ciphertext ct1(pt1);
  std::cout << ct1.get_label() << "\n";

  ct1 += ct1;
  ct1.set_as_output();
  
  fhecompiler::Ciphertext ct2 = ct1;
  std::cout << ct2.get_label() << "\n";
  return 0;
}
