#include "fheco/dsl/compiler_helper_functions.hpp"
#include <stdexcept>

using namespace std;

namespace fheco
{

void replace_all(string& str, const string& from, const string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Move past the replacement
    }
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

std::vector<std::string> process_vectorized_code(const string& content) {
    std::string cleaned_content = content;
    replace_all(cleaned_content, "(", "( ");
    replace_all(cleaned_content, ")", " )");
    replace_all(cleaned_content, "\n", " ");
    /********************************/
    istringstream iss(cleaned_content);
    vector<string> tokens;
    string token;

    while (iss >> token) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

bool is_literal(const std::string& token) {
    return std::none_of(token.begin(), token.end(), [](unsigned char c) { return std::isalpha(c); });
}

bool verify_all_vec_elems_eq0(const vector<string>& elems){
  bool all_vec_elems_eq0 = true ;
  for(auto elem : elems){
    if(elem!="0"){
      all_vec_elems_eq0=false;
    }
  }
  return all_vec_elems_eq0 ;
}

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
      if (elems.size() >= 2) {
          vec_ops1.emplace_back(elems[0]);
          vec_ops2.emplace_back(elems[1]);
      }else if(elems.size() == 1){
          vec_ops1.emplace_back(elems[0]);
      }
  }
  // Debug output to check results
}

string generate_rotated_expression(string& expression_to_rotate, int number_of_rotations, string operation) {
  string expression_builder = "";
  expression_to_rotate.erase(0, 1);   // remove the first char bcz it s a space
  
  string op = operation == "+" ? "+" : 
            operation == "-" ? "-" : 
            operation == "*" ? "*" : " ";
 
  if (number_of_rotations == 1) {
    expression_builder += " ( " + op + " " + expression_to_rotate + " ( << " + expression_to_rotate + " 1))";
  } else {
    expression_builder += " ( " + op + " " + expression_to_rotate;
    for (int idx = 1 ; idx <= number_of_rotations - 1; idx++) {
      expression_builder += " ( "+ op +" ( << " + expression_to_rotate + " " + std::to_string(idx) + " )";
    }
    expression_builder += " ( << " + expression_to_rotate + " " + std::to_string(number_of_rotations) + " )";
    
    for (int idx = 1 ; idx <= number_of_rotations; idx++) {
      expression_builder += " )";
    }
  }
  return expression_builder;
}

std::vector<std::string> split_string(const std::string& str, char delimiter){
    std::vector<std::string> substrings;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        substrings.push_back(token);
    }
    return substrings;
}

ir::OpCode operationFromString(string operation){
  if (operation == "+")
    return ir::OpCode::add;
  else if (operation == "-")
    return ir::OpCode::sub;
  else if (operation == "*")
    return ir::OpCode::mul;
  else if (operation == "square")
    return ir::OpCode::square;
  else
    throw logic_error("Invalid expression");
}

std::vector<int> split_string_ints(const std::string& str, char delimiter){
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

std::string vectorToString(const std::vector<int>& vec){
    std::ostringstream oss;
    
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i != 0) {
            oss << " ";  // Add a space before every element except the first
        }
        oss << vec[i];  // Add the element
    }
    
    return oss.str();
}

string constant_folding(queue<string> &tokens){
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
        bool is_op1_litteral = is_literal(operand1);
        bool is_op2_litteral = is_literal(operand2);
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

string convert_new_ops(queue<string> &tokens)
{
  while (!tokens.empty())
  {
    if (tokens.front() == "(")
    {
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

std::vector<std::string> tokenizeExpression(const std::string& expression) {
    std::vector<std::string> tokens;
    std::string currentToken;

    for (size_t i = 0; i < expression.size(); ++i) {
        char ch = expression[i];

        if (std::isalnum(ch) || ch == '_' || ch == '.') { 
            // Part of a token (alphanumeric or underscore/dot for identifiers like c_234 or 1.23)
            currentToken += ch;
        } else if (std::isspace(ch)) {
            // Space: End of current token (if any)
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
        } else {
            // Special characters like '(', ')', '*', etc.
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            tokens.emplace_back(1, ch); // Add the special character as a single token
        }
    }

    // Add the last token if any
    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }

    return tokens;
}
} // namespace fheco
