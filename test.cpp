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
std::string vectorToString(const std::vector<int>& vec) {
    std::ostringstream oss;
    
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i != 0) {
            oss << " ";  // Add a space before every element except the first
        }
        oss << vec[i];  // Add the element
    }
    
    return oss.str();
}
/***************************************************************************/
string vector_constant_folding(queue<string> &tokens,unordered_map<string,string>& input_entries, int& index)
{
  while (!tokens.empty())
  {
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
        operand1 = vector_constant_folding(tokens,input_entries,index);
      }
      else
      {
        operand1 = tokens.front();
        tokens.pop();
      }
      if (tokens.front() == "(")
      {
        operand2 = vector_constant_folding(tokens,input_entries,index);
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
        //std::cout<<operationString<<" "<<operand1<<" "<<operand2<<" \n";
        string type_op1 = operand1.substr(0,1);
        string type_op2 = operand2.substr(0,1);
        //std::cout<<type_op1<<" "<<type_op2<<" \n";
        if(type_op1=="p"&&type_op2=="p"){
            if (input_entries.find(operand1) == input_entries.end()){
              throw invalid_argument("given plaintext_label :"+operand1+" doesnt exist in input_entries");
            }
            if (input_entries.find(operand2) == input_entries.end()){
              throw invalid_argument("given plaintext_label :"+operand2+" doesnt exist in input_entries");
            }
            string header = input_entries.at(operand1).substr(0,4);
            vector<int> vec1 = split_string_ints(input_entries.at(operand1).substr(4),' ');
            vector<int> vec2 = split_string_ints(input_entries.at(operand2).substr(4),' ');
             if (vec1.size() != vec2.size()) {
               throw invalid_argument("vectors are of different lengths");
            }
            bool all_vec1_values_eq0 = true ;
            bool all_vec2_values_eq0 = true ;
            for(int i =0;i<vec1.size();i++){
               if(vec1[i]!=0){
                  all_vec1_values_eq0 = false ;
               }
               if(vec2[i]!=0){
                  all_vec2_values_eq0 = false ;
               }
            }
            if(all_vec1_values_eq0&&all_vec2_values_eq0){
                return operand1 ;
            }else if(all_vec1_values_eq0){
                return operand2 ;
            }else if(all_vec2_values_eq0){
               return operand1 ;
            }else{
              // Resize 'values' to match the size of 'vec1' (and 'vec2')
              vector<int> values ;
              values.resize(vec1.size());
              if (operationString=="+"){
                for(int i =0;i<vec1.size();i++){
                  values[i]=vec1[i]+vec2[i];
                }
              }else if(operationString=="-"){
                for(int i =0;i<vec1.size();i++){
                  values[i]=vec1[i]-vec2[i];
                }
              }else if(operationString=="*"){
                for(int i =0;i<vec1.size();i++){
                  values[i]=vec1[i]*vec2[i];
                }
              }
              string new_input = vectorToString(values);
              new_input=header+new_input;
              string label = "p" + std::to_string(index);
              index++;
              input_entries.insert({label,new_input});
              return label ;
            }
        }else if(type_op1=="p"){
            if (input_entries.find(operand1) == input_entries.end()){
              throw invalid_argument("given plaintext_label :"+operand1+" doesnt exist in input_entries");
            }
            string header = input_entries.at(operand1).substr(0,4);
            vector<int> vec1 = split_string_ints(input_entries.at(operand1).substr(4),' ');
            bool all_vec1_values_eq0 = true ;
            for(int i =0;i<vec1.size();i++){
               if(vec1[i]!=0){
                  all_vec1_values_eq0 = false ;
               }
            }
            if(all_vec1_values_eq0){
                if(operationString=="+"){
                    return operand2 ;
                }else if (operationString=="*"){
                    return operand1 ;
                }else if (operationString=="-"){
                    return "( "+operationString+" "+operand2+" )";
                }else if (operationString=="<<"){
                  return "";
                }
            }else{ 
              return "( "+operationString+" "+operand1+" "+operand2+" )" ;
            }
        }else if(type_op2=="p"){
            if (input_entries.find(operand2) == input_entries.end()){
              throw invalid_argument("given plaintext_label :"+operand2+" doesnt exist in input_entries");
            }
            string header = input_entries.at(operand2).substr(0,4);
            vector<int> vec2 = split_string_ints(input_entries.at(operand2).substr(4),' ');
            bool all_vec2_values_eq0 = true ;
            for(int i =0;i<vec2.size();i++){
               if(vec2[i]!=0){
                  all_vec2_values_eq0 = false ;
               }
            }
            if(all_vec2_values_eq0){
                if(operationString=="+"){
                    return operand1 ;
                }else if (operationString=="*"){
                    return operand2 ;
                }else if (operationString=="-"){
                    return operand1 ;
                }
            }else{ 
              return "( "+operationString+" "+operand1+" "+operand2+" )" ;
            }
        }else{
          return "( "+operationString+" "+operand1+" "+operand2+" )" ;
        }
      }
      else 
      {
        return " ( "+operationString+" "+operand1+" )" ;
      }
    }
    else
    {
      return tokens.front();
    }
  }
  throw logic_error("Invalid expressionnn");
}
/**********************************/
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
/***********************************/
std::vector<std::string> split_string(const std::string& str, char delimiter) {
    std::vector<std::string> substrings;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        substrings.push_back(token);
    }
    return substrings;
}
/*******************************************************************************/
/****************************Start subvector traitments***************************************************/
/*
1) separate vector elements into an elements vector
2) apply contant folding on each term 
3) check if all vector elements = 0 then delete this vector
repeat recusively : 
  1) separate the obtained vector into the (+ vector_elementary_elems vector_composed_elems)
  2) if vector vector_composed_elems == (0 0 0 ....0)
      return vector_elementary_elems
  3) separate vector_composed_elems into the sum of vectors with the same operand : 
      
*/
/****************************/
void replace_all(string& str, const string& from, const string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Move past the replacement
    }
}
/************************************************************************/
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
/***************************/
bool is_literal(const std::string& token) {
    return std::none_of(token.begin(), token.end(), [](unsigned char c) { return std::isalpha(c); });
}
/***************************************/
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
            }else{
               return "( "+operationString+" "+operand1+" "+operand2+" )" ;
            }
        }else{
            return "( "+operationString+" "+operand1+" "+operand2+" )" ;
        }
      }else{
        return " ( "+operationString+" "+operand1+" )" ;
      }
    }
    else
    {
      return tokens.front();
    }
  }
  throw logic_error("Invalid expression");
}
/**************************************/
bool verify_all_vec_elems_eq0(const vector<string>& elems){
  bool all_vec_elems_eq0 = true ;
  for(auto elem : elems){
    if(elem!="0"){
      all_vec_elems_eq0=false;
    }
  }
  return all_vec_elems_eq0 ;
}
/******************************************************************/
int id_counter = 0 ;
/*
std::pair<std::string, int> process(
    const std::vector<std::string>& tokens,
    int index,
    std::unordered_map<std::string, std::string>& dictionary,
    std::unordered_map<std::string, std::string>& inputs_entries,
    const std::vector<std::string>& inputs,
    const std::vector<std::string>& inputs_types,
    int slot_count,
    int sub_vector_size,
    string& new_expression
)
*/
string c
{
  if(!verify_all_vec_elems_eq0(vector_elements)){
    vector<string> simple_elements = {} ;
    vector<string> composed_elements = {} ;
    for(auto elem : vector_elements){
      if(elem.substr(0,1)=="("){
        composed_elements.push_back(elem);
        simple_elements.push_back("0");
      }else{
        composed_elements.push_back("0");
        simple_elements.push_back(elem);
      }
    }
    bool all_simple_elements_eq_0 = verify_all_vec_elems_eq0(simple_elements);
    bool all_composed_elements_eq_0 = verify_all_vec_elems_eq0(composed_elements);
    vector<string> addition_elements = {} ;
    vector<string> substraction_elements = {} ;
    vector<string> multiplication_elements = {} ;
    if(!all_composed_elements_eq_0){
      // declare simple_elements as a new ciphertext and store it 
      // indicate that they are associated with composed elements by an addition 
      // cout<<"divide composed_elements vector on three vectors each one containing\n";
      // cout<<"associated operations with + , - *\n";
      for(const auto elem : composed_elements){
        std::cout<<elem<<" --- ";
        if(elem.substr(1,2)==" +"){
          addition_elements.push_back(elem);
          substraction_elements.push_back("0");
          multiplication_elements.push_back("0");
        }else if(elem.substr(1,2)==" -"){
          addition_elements.push_back("0");
          substraction_elements.push_back(elem);
          multiplication_elements.push_back("0");
        }else if(elem.substr(1,2)==" *"){
          addition_elements.push_back("0");
          substraction_elements.push_back("0");
          multiplication_elements.push_back(elem);
        }else if(elem=="0"){
          addition_elements.push_back("0");
          substraction_elements.push_back("0");
          multiplication_elements.push_back("0");
        }
      }
      /**
      std::cout<<"\n After decomposition ==> \n";
      cout<<"Addition :\n";
      for(auto val : addition_elements)
        std::cout<<val<<" --- ";
      cout<<"\n\nSubtraction :\n";
      for(auto val : substraction_elements)
        std::cout<<val<<" --- ";
      cout<<"\n\nMultiplication :\n";
      for(auto val : multiplication_elements)
        std::cout<<val<<" --- ";
      cout<<"\n\n";
      /*****************************************/
      //std::cout<<"additon_elems :"<<verify_all_vec_elems_eq0(addition_elements)<<"substraction_elems :"<<verify_all_vec_elems_eq0(substraction_elements)<<"mutliplication :"<<verify_all_vec_elems_eq0(multiplication_elements)<<"\n";
      if(!verify_all_vec_elems_eq0(addition_elements)&&!verify_all_vec_elems_eq0(substraction_elements)&&!verify_all_vec_elems_eq0(multiplication_elements)){
        return "( + "+process_composed_vectors(addition_elements)+" ( + "+process_composed_vectors(substraction_elements)+" "+process_composed_vectors(multiplication_elements)+" ) )";
      }else if(!verify_all_vec_elems_eq0(addition_elements)&&!verify_all_vec_elems_eq0(substraction_elements)){
         return "( + "+process_composed_vectors(addition_elements)+" "+process_composed_vectors(substraction_elements)+" )";
      }else if(!verify_all_vec_elems_eq0(substraction_elements)&&!verify_all_vec_elems_eq0(multiplication_elements)){
        return "( + "+process_composed_vectors(substraction_elements)+" "+process_composed_vectors(multiplication_elements)+" )";
      }else if(!verify_all_vec_elems_eq0(addition_elements)&&!verify_all_vec_elems_eq0(multiplication_elements)){
        return "( + "+process_composed_vectors(addition_elements)+" "+process_composed_vectors(multiplication_elements)+" )";
      }else if(!verify_all_vec_elems_eq0(addition_elements)){
        vector<string> vec_ops1 ={} ;
        vector<string> vec_ops2 ={} ;
        for(auto elem : addition_elements){
          vector<string> elems = {};
          if(elem=="0"){
            vec_ops1.push_back(elem);
            vec_ops2.push_back(elem);
          }else{
            elem=elem.substr(3);
            elem=elem.substr(0,elem.size()-2);
            istringstream iss(elem);
            string vector_string_element="";
            string element="";
            while (iss >> element) {
                if (element=="("){
                    vector_string_element+=" (" ;
                    //string sub_expression="";
                    int sub_nested_level=0 ;
                    while (iss >> element&&sub_nested_level>=0){
                        if(element!=")"&&element!="("){
                            vector_string_element+=" "+element; 
                        }else{
                            sub_nested_level += (element == "(") ? 1 : (element == ")") ? -1 : 0;
                            vector_string_element+=" "+element ;
                        }
                    }
                    iss.seekg(-element.length(), std::ios_base::cur);
                    elems.push_back(vector_string_element.substr(1,vector_string_element.size()));
                }else{
                    elems.push_back(element);
                }     
            }
            vec_ops1.push_back(elems[0]);
            vec_ops2.push_back(elems[1]);
          }
        }
        return "( + "+process_composed_vectors(vec_ops1)+" "+process_composed_vectors(vec_ops2)+" )";
      }else if(!verify_all_vec_elems_eq0(substraction_elements)){
        vector<string> vec_ops1 ={} ;
        vector<string> vec_ops2 ={} ;
        for(auto elem : substraction_elements){
          vector<string> elems = {};
          if(elem=="0"){
            vec_ops1.push_back(elem);
            vec_ops2.push_back(elem);
          }else{
            elem=elem.substr(3);
            elem=elem.substr(0,elem.size()-2);
            istringstream iss(elem);
            string vector_string_element="";
            string element="";
            while (iss >> element) {
                if (element=="("){
                    vector_string_element+=" (" ;
                    //string sub_expression="";
                    int sub_nested_level=0 ;
                    while (iss >> element&&sub_nested_level>=0){
                        if(element!=")"&&element!="("){
                            vector_string_element+=" "+element; 
                        }else{
                            sub_nested_level += (element == "(") ? 1 : (element == ")") ? -1 : 0;
                            vector_string_element+=" "+element ;
                        }
                    }
                    iss.seekg(-element.length(), std::ios_base::cur);
                    elems.push_back(vector_string_element.substr(1,vector_string_element.size()));
                }else{
                    elems.push_back(element);
                }     
            }
            vec_ops1.push_back(elems[0]);
            vec_ops2.push_back(elems[1]);
          }
        }
        return "( - "+process_composed_vectors(vec_ops1)+" "+process_composed_vectors(vec_ops2)+" )";
      }else if(!verify_all_vec_elems_eq0(multiplication_elements)){
        vector<string> vec_ops1 ={} ;
        vector<string> vec_ops2 ={} ;
        for(auto elem : multiplication_elements){
          vector<string> elems = {};
          if(elem=="0"){
            vec_ops1.push_back(elem);
            vec_ops2.push_back(elem);
          }else{
            elem=elem.substr(3);
            elem=elem.substr(0,elem.size()-2);
            istringstream iss(elem);
            string vector_string_element="";
            string element="";
            while (iss >> element) {
                if (element=="("){
                    vector_string_element+=" (" ;
                    //string sub_expression="";
                    int sub_nested_level=0 ;
                    while (iss >> element&&sub_nested_level>=0){
                        if(element!=")"&&element!="("){
                            vector_string_element+=" "+element; 
                        }else{
                            sub_nested_level += (element == "(") ? 1 : (element == ")") ? -1 : 0;
                            vector_string_element+=" "+element ;
                        }
                    }
                    iss.seekg(-element.length(), std::ios_base::cur);
                    elems.push_back(vector_string_element.substr(1,vector_string_element.size()));
                }else{
                    elems.push_back(element);
                }     
            }
            vec_ops1.push_back(elems[0]);
            vec_ops2.push_back(elems[1]);
          }
        }
        return "( * "+process_composed_vectors(vec_ops1)+" "+process_composed_vectors(vec_ops2)+" )";
      }
    }
    if(!all_simple_elements_eq_0){
      string new_element = ""; 
      bool is_literal_val = true ;
      for(auto val :simple_elements){
        if(!is_literal(val)){
          if (inputs_types[std::distance(inputs.begin(), std::find(inputs.begin(), inputs.end(), element))] == "1") {
            is_literal_val = false;
          }
        }
        new_element+=val+" ";
      }
      string string_vector = "Vec "+new_element.substr(0, new_element.size() - 1);// strip trailing space
      if (dictionary.find(string_vector) == dictionary.end()) {
        int number_of_sub_vectors = slot_count / sub_vector_size;
        string res = "";
        for(int i =0 ; i<number_of_sub_vectors ; i++){
            res+=new_element;
        }
        new_element = res ;
        new_element = new_element.substr(0, new_element.size() - 1); 
        string label;
        if(is_literal_val){
          label = "p" + std::to_string(id_counter);
          new_element = "1 1 " + new_element;
          std::cout<<label<<" : "<<new_element<<" \n";
        }else{
          label = "c" + std::to_string(id_counter);
          new_element = "1 1 " + new_element;
          std::cout<<label<<" : "<<new_element<<" \n";
        }
        new_inputs_labels.push_back(label);
        labels_map[id_counter] = label;
        inputs_entries[label]=new_element;
        id_counter++ ;
        dictionary[string_vector] = label;
        return label ;
      }else{
        return dictionary[string_vector]
      }
    }
  }
  else{
    string new_element = ""; 
    for(auto val : updated_vector_elements){
      new_element+=val+" ";
    }
    string string_vector = "Vec "+new_element.substr(0, new_element.size() - 1);// strip trailing space
    if (dictionary.find(string_vector) == dictionary.end()) {
      int number_of_sub_vectors = slot_count / sub_vector_size;
      string res = "";
      for(int i =0 ; i<number_of_sub_vectors ; i++){
          res+=new_element;
      }
      new_element = res ;
      new_element = new_element.substr(0, new_element.size() - 1); 
      string label;
      label = "p" + std::to_string(id_counter);
      new_element = "0 1 " + new_element;
      new_inputs_labels.push_back(label);
      labels_map[id_counter] = label;
      inputs_entries[label]=new_element;
      id_counter++ ;
      dictionary[string_vector] = label;
      return label ;
    }else{
      return dictionary[string_vector]
    }
  }
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
  string expr = "Vec (* in_159 -1) (* in_1014 0) (+ (+ (+ (* in_1411 0) (+ (+ (* in_1312 1) (+ (* in_1311 0) (+ 0 (* in_1310 -1)))) (* in_1410 -2))) (* in_1412 2)) (* in_1510 -1)) (* in_915 0) (* in_1111 -1) (+ (+ (+ (* in_1013 0) (+ (+ (* in_914 1) (+ (* in_913 0) (+ 0 (* in_912 -1)))) (* in_1012 -2))) (* in_1014 2)) (* in_1112 -1)) (+ (+ (+ (* in_1014 0) (+ (+ (* in_915 1) (+ (* in_914 0) (+ 0 (* in_913 -1)))) (* in_1013 -2))) (* in_1015 2)) (* in_1113 -1)) (* in_1015 0)) ";
  //string expr1 = "Vec (* in_159 0) (* in_1014 0) (* in_1014 0)) ";
  auto tokens = process_vectorized_code(expr);
  int index = 0 ;
  string vector_string="";
  if (tokens[index] == "Vec"){
    vector_string = "Vec ";
    int nested_level = 0;
    index++;
    while (nested_level >= 0) {
        vector_string += tokens[index] + " ";
        nested_level += (tokens[index] == "(") ? 1 : (tokens[index] == ")") ? -1 : 0;
        index++;
    }
  }
  /*******************************************************************/
  vector_string = vector_string.substr(0, vector_string.size() - 2);  // Strip trailing space and closing parenthesis
  istringstream iss(vector_string.substr(4));
  vector<string> vector_elements = {};
  string element="";
  while (iss >> element) {
    string vector_string_element="";
    if (element=="("){
      vector_string_element+=" (" ;
      //string sub_expression="";
      int sub_nested_level=0 ;
      while (iss >> element&&sub_nested_level>=0){
          if(element!=")"&&element!="("){
            vector_string_element+=" "+element; 
          }else{
              sub_nested_level += (element == "(") ? 1 : (element == ")") ? -1 : 0;
              vector_string_element+=" "+element ;
          }
      }
      iss.seekg(-element.length(), std::ios_base::cur);
      vector_elements.push_back(vector_string_element.substr(1,vector_string_element.size()));
    }else{
      vector_elements.push_back(element);
    }     
  }
  /*****************************************/
  string res = process_composed_vectors(vector_elements);
  std::cout<<"\n\n==> :"<<res<<"\n";
  /*****************************************************************
  vector<string> updated_vector_elements = {};
  cout<<"Applying constant Folding ==> \n";
  bool if_all_vector_elems_eq0 = true ;
  for(auto elem : vector_elements){
    auto tokens = split(elem);
    string updated_elem = constant_folding(tokens);
    updated_vector_elements.push_back(updated_elem);
  }
 
  if(!verify_all_vec_elems_eq0(updated_vector_elements)){
    cout<<"Separate Simple and composed elements==>\n";
    vector<string> simple_elements = {} ;
    vector<string> composed_elements = {} ;
    for(auto elem : updated_vector_elements){
      if(elem.substr(0,1)=="("){
        composed_elements.push_back(elem);
        simple_elements.push_back("0");
      }else{
        composed_elements.push_back("0");
        simple_elements.push_back(elem);
      }
    }
    bool all_simple_elements_eq_0 = verify_all_vec_elems_eq0(simple_elements);
    bool all_composed_elements_eq_0 = verify_all_vec_elems_eq0(composed_elements);
    vector<string> addition_elements = {} ;
    vector<string> substraction_elements = {} ;
    vector<string> multiplication_elements = {} ;
    if(!all_composed_elements_eq_0){
      // declare simple_elements as a new ciphertext and store it 
      // indicate that they are associated with composed elements by an addition 
      // divide composed_elements vector on three vectors each one containing 
      // associated operations with + , - * 
      for(auto elem : composed_elements){
        string res = elem.substr(1,2);
        if(elem.substr(1,2)==" +"){
          addition_elements.push_back(elem);
          substraction_elements.push_back("0");
          multiplication_elements.push_back("0");
        }else if(elem.substr(1,2)==" -"){
          addition_elements.push_back("0");
          substraction_elements.push_back(elem);
          multiplication_elements.push_back("0");
        }else if(elem.substr(1,2)==" *"){
          addition_elements.push_back("0");
          substraction_elements.push_back("0");
          multiplication_elements.push_back(elem);
        }
      }
      if(!verify_all_vec_elems_eq0(addition_elements)&&!verify_all_vec_elems_eq0(substraction_elements)&&!verify_all_vec_elems_eq0(multiplication_elements)){
        return "( + "+process_composed_vectors(addition_elements)+" ( + "+process_composed_vectors(substraction_elements)+" "+process_composed_vectors(multiplication_elements)+" ) )";
      }else if(!verify_all_vec_elems_eq0(addition_elements)&&!verify_all_vec_elems_eq0(substraction_elements)){
         return "( + "+process_composed_vectors(addition_elements)+" "+process_composed_vectors(substraction_elements)+" )";
      }else if(!verify_all_vec_elems_eq0(substraction_elements)&&!verify_all_vec_elems_eq0(multiplication_elements)){
        return "( + "+process_composed_vectors(substraction_elements)+" "+process_composed_vectors(multiplication_elements)+" )";
      }else if(!verify_all_vec_elems_eq0(addition_elements)&&!verify_all_vec_elems_eq0(multiplication_elements)){
        return "( + "+process_composed_vectors(addition_elements)+" "+process_composed_vectors(multiplication_elements)+" )";
      }else if(!verify_all_vec_elems_eq0(addition_elements)){
        vector<string> vec_ops1 ={} ;
        vector<string> vec_ops2 ={} ;
        for(auto elem : addition_elements){
          vector<string> elems = {};
          elem=elem.substr(3);
          elem=elem.substr(0,elem.size()-2);
          istringstream iss(elem);
          string vector_string_element="";
          string element="";
          while (iss >> element) {
              if (element=="("){
                  vector_string_element+=" (" ;
                  //string sub_expression="";
                  int sub_nested_level=0 ;
                  while (iss >> element&&sub_nested_level>=0){
                      if(element!=")"&&element!="("){
                          vector_string_element+=" "+element; 
                      }else{
                          sub_nested_level += (element == "(") ? 1 : (element == ")") ? -1 : 0;
                          vector_string_element+=" "+element ;
                      }
                  }
                  iss.seekg(-element.length(), std::ios_base::cur);
                  elems.push_back(vector_string_element.substr(1,vector_string_element.size()));
              }else{
                  elems.push_back(element);
              }     
          }
          vec_ops1.push_back(elems[0]);
          vec_ops2.push_back(elems[1]);
        }
        return "( + "+process_composed_vectors(vec_ops1)+" "+process_composed_vectors(vec_ops2)+" )";

      }else if(!verify_all_vec_elems_eq0(substraction_elements)){
        vector<string> vec_ops1 ={} ;
        vector<string> vec_ops2 ={} ;
        for(auto elem : substraction_elements){
          vector<string> elems = {};
          elem=elem.substr(3);
          elem=elem.substr(0,elem.size()-2);
          istringstream iss(elem);
          string vector_string_element="";
          string element="";
          while (iss >> element) {
              if (element=="("){
                  vector_string_element+=" (" ;
                  //string sub_expression="";
                  int sub_nested_level=0 ;
                  while (iss >> element&&sub_nested_level>=0){
                      if(element!=")"&&element!="("){
                          vector_string_element+=" "+element; 
                      }else{
                          sub_nested_level += (element == "(") ? 1 : (element == ")") ? -1 : 0;
                          vector_string_element+=" "+element ;
                      }
                  }
                  iss.seekg(-element.length(), std::ios_base::cur);
                  elems.push_back(vector_string_element.substr(1,vector_string_element.size()));
              }else{
                  elems.push_back(element);
              }     
          }
          vec_ops1.push_back(elems[0]);
          vec_ops2.push_back(elems[1]);
        }
        return "( - "+process_composed_vectors(vec_ops1)+" "+process_composed_vectors(vec_ops2)+" )";
      }else if(!verify_all_vec_elems_eq0(multiplication_elements)){
        vector<string> vec_ops1 ={} ;
        vector<string> vec_ops2 ={} ;
        for(auto elem : multiplication_elements){
          vector<string> elems = {};
          elem=elem.substr(3);
          elem=elem.substr(0,elem.size()-2);
          istringstream iss(elem);
          string vector_string_element="";
          string element="";
          while (iss >> element) {
              if (element=="("){
                  vector_string_element+=" (" ;
                  //string sub_expression="";
                  int sub_nested_level=0 ;
                  while (iss >> element&&sub_nested_level>=0){
                      if(element!=")"&&element!="("){
                          vector_string_element+=" "+element; 
                      }else{
                          sub_nested_level += (element == "(") ? 1 : (element == ")") ? -1 : 0;
                          vector_string_element+=" "+element ;
                      }
                  }
                  iss.seekg(-element.length(), std::ios_base::cur);
                  elems.push_back(vector_string_element.substr(1,vector_string_element.size()));
              }else{
                  elems.push_back(element);
              }     
          }
          vec_ops1.push_back(elems[0]);
          vec_ops2.push_back(elems[1]);
        }
        return "( * "+process_composed_vectors(vec_ops1)+" "+process_composed_vectors(vec_ops2)+" )";
      }
    }if(!all_simple_elements_eq_0){
      string new_element = ""; 
      for(auto val :simple_elements){
        new_element+=val+" ";
      }
      new_element = new_element.substr(0, new_element.size() - 1); 
      string label;
      label = "c" + std::to_string(id_counter);
      new_element = "1 1 " + new_element;
      //new_inputs_labels.push_back(label);
      //labels_map[id_counter] = label;
      //inputs_entries[label]=new_element;
      id_counter++ ;
      return label ;
      // create the new element and store corresponding value in dictionary 
      // increent counter-id 
    }
  }{
    string label;
    label = "p" + std::to_string(id_counter);
    //new_element = "0 1 " + new_element;
    //new_inputs_labels.push_back(label);
    //labels_map[id_counter] = label;
    //inputs_entries[label]=new_element;
    id_counter++ ;
    return label ;
  }***/
  
  
  
  /*string IR = "( + ( * p98 c77 ) ( + ( - p104 p103 ) p95 )";
  unordered_map<string,string> input_entries = {} ;
  /******************************************
  input_entries.insert({"p98","1 1 0 0 0 0"});
  input_entries.insert({"p103","1 1 1 0 0 1"});
  input_entries.insert({"p104","1 1 0 0 0 1"});
  input_entries.insert({"p95","1 1 0 0 0 0"});
  input_entries.insert({"c77","1 1 v1 v2 0 0"});
  input_entries.insert({"c78","1 1 v3 0 0 0"});
  std::cout<<input_entries.size()<<"\n\n";
  /*******************************************
  if (input_entries.find("p98") == input_entries.end()){
    throw invalid_argument("given plaintext_label doesnt exist in input_entries");
  }else{
    vector<int> values =split_string_ints(input_entries.at("p98"),' ');
  }
  auto tokens1 = split(IR);
  int index = 105 ;
  string res = vector_constant_folding(tokens1,input_entries,index);
  if(res==""){
    throw invalid_argument("obtained ir is empty");
  }else{
    std::cout<<res<<" \n";
  }
  /***************************
  vector<string> tokens = split_string(res,' ');
  vector<string> labels = {};
  for (const auto& pair : input_entries) {
    labels.push_back(pair.first);  // Access the key via pair.first
  }
  unordered_map<string,int> inputs_occurences ={};
  for(auto label : labels){
      inputs_occurences.insert({label,0});
  }
  for(int i =0;i<tokens.size();i++){
    for(auto label : labels){
        if(label==tokens[i]){
          inputs_occurences[label]+=1;
        }
    }
  }
  for(auto label : labels){
      if(inputs_occurences[label]==0){
        input_entries.erase(label);
      }
  }
  /***************End************
  std::cout<<"Remaining elements \n";
  for(const auto& pair : input_entries){
    std::cout<<pair.first<<" \n";
  }
  /*******************************/
} 
/**
template <typename KeyType, typename ValueType>
class InsertionOrderedMap {
private:
    std::unordered_map<KeyType, ValueType,typename KeyType::HashPtr, typename KeyType::EqualPtr> map_;
    std::vector<KeyType> insertion_order_;

public:
    // Insert an element in the map, and track insertion order
    void insert(const KeyType& key, const ValueType& value) {
        // Check if key already exists
        if (map_.find(key) == map_.end()) {
            // If key is new, record the insertion order
            insertion_order_.push_back(key);
        }
        // Insert or update the key-value pair in the unordered_map
        map_[key] = value;
    }

    // Retrieve a value based on the key
    ValueType get(const KeyType& key) const {
        return map_.at(key);
    }

    // Check if a key exists
    bool contains(const KeyType& key) const {
        return map_.find(key) != map_.end();
    }

    // Iterate through elements in insertion order
    void print_in_order() const {
        for (const auto& key : insertion_order_) {
            //std::cout << key << ": " << map_.at(key) << std::endl;
        }
    }
};

class Info {
  public : 
  Info(int v) : val(v) {}
  int val ;
  bool operator==(const Info &other) const {
        return val == other.val;
  }

  struct HashPtr
  {
    std::size_t operator()(const Info *p) const{
      return std::hash<int>()(p->val);
    }
  };
  struct EqualPtr
  {
    bool operator()(const Info *lhs, const Info *rhs) const{
        return *lhs == *rhs;
    }
  };
};
using Infostrings = InsertionOrderedMap<Info,string> ;
int main() {
    Infostrings my_map;
    Info inf(1);
    my_map.insert(inf, "two");
    Info inf2(2);
    my_map.insert(inf2, "one");
    Info inf3(3);
    my_map.insert(inf3, "three");

    std::cout << "Elements in insertion order:\n";
    my_map.print_in_order();
    // Add another element
    Info inf4(4);
    my_map.insert(inf4, "four");
    std::cout << "\nAfter inserting one more element:\n";
    my_map.print_in_order();

    return 0;
}
Func.cpp:
data_flow_.set_output(term, OutputTermInfo{unordered_set<string>{move(label)}, output.example_val()});
expr.cpp :
void Expr::set_output(const Term *term, OutputTermInfo output_term_info)
{
  if (outputs_info_.insert_or_assign(term, move(output_term_info)).second)
    valid_top_sort_ = false;
}
struct NoiseEstimatesValue
{
  int fresh_noise = 0;
  int mul_noise_growth = 0;
  int mul_plain_noise_growth = 0;
};

template <typename Iter>
void gen_sequence(Iter begin, Iter end, std::size_t line_threshold, std::ostream &os)
{
  if (begin == end)
    return;

  size_t i = 0;
  for (Iter it = begin;;)
  {
    os << *it;
    ++it;
    ++i;
    if (it == end)
      break;

    if (i == line_threshold)
    {
      os << ",\n";
      i = 0;
    }
    else
      os << ", ";
  }
}
/********************************** */
(Vec ( + in_33 ( + in_32 ( + in_31 ( + in_23 ( + in_22 ( + in_21 ( + in_13 ( + in_12 ( + in_11 0 ) ) ) ) ) ) ) ) ) ( + in_32 ( + in_31 ( + in_30 ( + in_22 ( + in_21 ( + in_20 ( + in_12 ( + in_11 ( + in_10 0 ) ) ) ) ) ) ) ) ) ( + in_33 ( + in_32 ( + in_31 ( + in_23 ( + in_22 ( + in_21 0 ) ) ) ) ) ) ( + in_23 ( + in_22 ( + in_13 ( + in_12 ( + in_03 ( + in_02 0 ) ) ) ) ) ) ( + in_23 ( + in_22 ( + in_21 ( + in_13 ( + in_12 ( + in_11 ( + in_03 ( + in_02 ( + in_01 0 ) ) ) ) ) ) ) ) ) ( + in_22 ( + in_21 ( + in_20 ( + in_12 ( + in_11 ( + in_10 ( + in_02 ( + in_01 ( + in_00 0 ) ) ) ) ) ) ) ) ) ( + in_21 ( + in_20 ( + in_11 ( + in_10 ( + in_01 ( + in_00 0 ) ) ) ) ) ) ( + in_13 ( + in_12 ( + in_03 ( + in_02 0 ) ) ) )  )
