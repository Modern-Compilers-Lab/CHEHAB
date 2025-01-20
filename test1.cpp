#include<iostream>
#include <vector>
#include <sstream>
#include <queue>
#include <cstring> 
#include <ctime>
#include <filesystem>
#include <fstream> 
#include <iostream>
#include <map>
#include <ostream>
#include <queue>
#include <algorithm>
using namespace std ;

/********************************************/
bool is_literal(const std::string& token) {
    return std::none_of(token.begin(), token.end(), [](unsigned char c) { return std::isalpha(c); });
}
/*********************************************/
string constant_folding(queue<string> &tokens)
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
        operand1 = constant_folding(tokens);
      }
      else
      {
        //std::cout<<"get op1 \n";
        operand1 = tokens.front();
        tokens.pop();
      }
      if (tokens.front() == "(")
      {
        operand2 = constant_folding(tokens);
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
        //std::cout<<operationString<<" "<<operand1<<" "<<operand2<<" \n";
        bool is_op1_litteral = is_literal(operand1);
        bool is_op2_litteral = is_literal(operand2);
        //std::cout<<type_op1<<" "<<type_op2<<" \n";
        if(is_op1_litteral&&is_op2_litteral){
          int op1 = 0;
          int op2 = 0;
          try{
            op1 = stoi(operand1);
            op2 = stoi(operand2); 
            int res = 0 ;
            if(operationString=="+"){
              res = op1+op2 ;
            }else if(operationString=="-"){
              res = op1-op2 ;
            }else if(operationString=="*"){
              res = op1*op2 ;
            }
            string res_op = std::to_string(res);
            return res_op ;
          }catch(exception e){
            throw invalid_argument("value :"+operand1+" or "+operand2+" cant be converted to int");
          }
        }else if(is_op1_litteral){
            int op1 = stoi(operand1);
            int res = 0;
            if(op1==0){
              if(operationString=="+"){
                return operand2;
              }else if(operationString=="-"){
                return "( "+operationString+" "+operand2+" )";
              }else if(operationString=="*"){
                return std::to_string(res);
              }
            }else if (op1==1){
              if(operationString=="*"){
                return operand2;
              }else{
                return "( "+operationString+" "+operand1+" "+operand2+" )" ;
              }
            }else if (op1==-1){
             if(operationString=="*"){
                return "( - "+operand2+" )";
              }else{
                return "( "+operationString+" "+operand1+" "+operand2+" )" ;
              }
            }else{
               return "( "+operationString+" "+operand1+" "+operand2+" )" ;
            }
        }else if (is_op2_litteral){
            //std::cout<<"welcome \n";
            int op2 = stoi(operand2);
            int res = 0;
            if(op2==0){
              if(operationString=="+"){
                return operand1;
              }else if(operationString=="-"){
                return "( "+operationString+" "+operand1+" )";
              }else if(operationString=="*"){
                return std::to_string(res);
              }
            }else if (op2==1){
              if(operationString=="*"){
                return operand1;
              }else{
                return "( "+operationString+" "+operand1+" "+operand2+" )" ;
              }
            }else if (op2==-1){
             if(operationString=="*"){
                return "( - "+operand1+" )";
              }else{
                return "( "+operationString+" "+operand1+" "+operand2+" )" ;
              }
            }
            else{
               return "( "+operationString+" "+operand1+" "+operand2+" )" ;
            }
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
/*********************************************************/
#include <cctype>

bool isSingleOperandExpression(const std::string& expression) {
    // Extract the content inside the outer parentheses
    std::string content = expression.substr(4, expression.size()); // Removes "( - " and ")"
    int depth = 0; // Tracks the current depth of parentheses
    int comp = 0 ;
    istringstream iss(content);
    string token;
    vector<string> tokens = {};
    while (iss >> token) {
      tokens.push_back(token);
    }
    for (auto c : tokens) {
        if (c == "(") {
            depth++; // Entering a nested expression
        } else if (c == ")") {
            depth--; // Exiting a nested expression
        }
        if(depth == 0){
          break;
        }
        comp++ ;
    }
    bool is_unary = false ;
    if(tokens[comp+1]==")")
      is_unary = true ;
    return is_unary ;
}
/*******************************************************************/
void decompose_vector_op(const vector<string>& vector_elements, vector<string>& vec_ops1 , vector<string>& vec_ops2){
   vector<string> elems = {};
   for (auto elem : vector_elements) {
        vector<string> elems;
        // Check for a "0" element
        if (elem == "0") {
            vec_ops1.emplace_back(elem);
            vec_ops2.emplace_back(elem);
            continue;
        }
        // Remove the first "( + " and last " )"
        elem = elem.substr(3, elem.size() - 5);
        // Stream processing
        istringstream iss(elem);
        string token, nested_expr;
        int nested_level = 0;
        while (iss >> token) {
          
            if (token == "(") {
                // Start a new nested expression
                nested_expr.clear();
                nested_expr += token;
                nested_level = 1;
                // Collect tokens until the nested level returns to zero
                while (nested_level > 0 && iss >> token) {
                    nested_expr += " " + token;
                    if (token == "(") ++nested_level;
                    else if (token == ")") --nested_level;
                }
                elems.emplace_back(nested_expr);
            } else {
                // Simple operand
                elems.emplace_back(token);
            }
        }
        if (elems.size() == 2) {
            vec_ops1.emplace_back(elems[0]);
            vec_ops2.emplace_back(elems[1]);
        }else if(elems.size() == 1){
            vec_ops1.emplace_back(elems[0]);
        }
    }
    // Debug output to check results
    /*cout << "vec_ops1: ";
    for (const auto& op : vec_ops1) cout << op << " --- ";
    cout << "\nvec_ops2: ";
    for (const auto& op : vec_ops2) cout << op << " --- ";*/
}
/******************************************************************/
int main(){
    string temp_elem ="( - in_2_0 )";
    auto tokens = split(temp_elem);
    temp_elem=constant_folding(tokens);
    std::cout<<temp_elem[3]<<" \n";
    std::string expression = "( - in_2_0 )";
    std::cout<<expression[0]<<" \n";
    // Check if the first character is an opening parenthesis
    if (!expression.empty() && expression[0] == '(') {
        std::cout << "The first character is an opening parenthesis." << std::endl;
    } else {
        std::cout << "The first character is NOT an opening parenthesis." << std::endl;
    }
    /*******************************************
    std::string expr1 = "( - ( * in2 in4 ) )";
    std::string expr2 = "( - in1 )";
    if (isSingleOperandExpression(expr1)) {
        std::cout << expr1 << " is a single operand expression." << std::endl;
    } else {
        std::cout << expr1 << " is a multiple operand expression." << std::endl;
    }

    if (isSingleOperandExpression(expr2)) {
        std::cout << expr2 << " is a single operand expression." << std::endl;
    } else {
        std::cout << expr2 << " is a multiple operand expression." << std::endl;
    }
    /****************************************************************
    vector<string> addition_elements = {"( - a1 )","( - a3 )"};
    vector<string> vec_ops1 ={} ;
    vector<string> vec_ops2 ={} ;
    decompose_vector_op(addition_elements, vec_ops1 ,vec_ops2);
    for(auto val : vec_ops1){
      std::cout<<val<<" ";
    }
    std::cout<<"\n";
     for(auto val : vec_ops2){
      std::cout<<val<<" ";
    }
    /****************************************************************/
    string new_term_str ="( + ( + c1 ( - c0 ) ) ( + c4 ( - c3 ) ) )";
                          ( + ( + c1 ( - c0 ) ) ( + c4 ( - c3 ) ) )
    tokens = split(new_term_str);
    while (!tokens.empty())
    {
       std::cout<<tokens.front()<<"";
       tokens.pop();
    }
    std::cout<<"\n";
    return 0;    
}