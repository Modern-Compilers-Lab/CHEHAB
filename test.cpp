#include<iostream>
#include<map>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <functional>

#include <unordered_map>
#include <vector>
#include <queue>
#include <sstream>
#include <algorithm>
#define MOD_BIT_COUNT_MAX 60 

using namespace std ; 

std::vector<int> split_string_ints(const std::string& str, char delimiter) {
    std::vector<int> composingValues;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
      try{
          composingValues.push_back(stoi(token));
      }catch(exception e){
        throw invalid_argument("value :"+token+" cant be converted to int");
      }
    }
    return composingValues;
}
/************************************************************************/
string convert_new_ops(queue<string> &tokens)
{
  //std::cout<<"welcome in constant folding\n";
  while (!tokens.empty())
  {
    //std::cout<<"hereee :"<<tokens.front()<<"\n";
    if (tokens.front() == "(")
    {
      //std::cout<<"here\n";
      tokens.pop();
      string operationString = tokens.front();
      tokens.pop();
      string potential_step = "";
      string operand1="" ,operand2="";
      if (tokens.front() == "(")
      {
        operand1 = convert_new_ops(tokens);
      }
      else
      {
        operand1 = tokens.front();
        tokens.pop();
      }
      if (tokens.front() == "(")
      {
        operand2 = convert_new_ops(tokens);
        potential_step += " ";

      }
      else if (tokens.front() != ")")
      {
        //std::cout<<"get op2 \n";
        operand2 = tokens.front();
        potential_step = tokens.front();
        tokens.pop();
      }

      // Check for the closing parenthesis
      if (tokens.front() == ")")
      {
        tokens.pop();
      }
      if (potential_step.size() > 0)
      {
        if (operationString=="VecAddRot"){
            return "( + "+operand1+" ( << "+operand1+" "+operand2+" ) )" ;
        }else if (operationString=="VecMinusRot"){
            return "( - "+operand1+" ( << "+operand1+" "+operand2+" ) )" ;
        }else if (operationString=="VecMulRot"){
            return "( * "+operand1+" ( << "+operand1+" "+operand2+" ) )" ;
        }else{
          return "( "+operationString+" "+operand1+" "+operand2+" )" ;
        }
      }else{
        return "( "+operationString+" "+operand1+" )" ;
      }
    }
    else
    {
      return tokens.front();
    }
  }
  throw logic_error("Invalid expression");
}
/************************************************************************/
queue<string>split(const string &s)
{
  queue<std::string> tokens;
  stringstream ss(s);
  std::string token;
  while (getline(ss, token, ' '))
  {
    tokens.push(token);
  }
  return tokens;
}
/******************************************************************/
int main(){
  /**before 
   while (nested_level >= 0) {
    string_vector += tokens[index] + " ";
    nested_level += (tokens[index] == "(") ? 1 : (tokens[index] == ")") ? -1 : 0;
    index++;
  }
  */
  string expr = "( VecMinusRot ( VecMulRot c1 4 ) 2 )";
  auto tokens = split(expr);
  string updated_elem = convert_new_ops(tokens);
  std::cout<<"===> "<<updated_elem<<" \n";
}