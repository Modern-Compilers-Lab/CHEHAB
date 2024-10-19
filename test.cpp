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
/*******************************************************************************/
int main(){
  string IR = "( + ( * p98 c77 ) ( + ( - p104 p103 ) p95 )";
  unordered_map<string,string> input_entries = {} ;
  /******************************************/
  input_entries.insert({"p98","1 1 0 0 0 0"});
  input_entries.insert({"p103","1 1 1 0 0 1"});
  input_entries.insert({"p104","1 1 0 0 0 1"});
  input_entries.insert({"p95","1 1 0 0 0 0"});
  input_entries.insert({"c77","1 1 v1 v2 0 0"});
  input_entries.insert({"c78","1 1 v3 0 0 0"});
  std::cout<<input_entries.size()<<"\n\n";
  /*******************************************/
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
  /***************************/
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
  /***************End************/
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
( + ( * ( + ( * ( * ( + ( * c0 c1 ) ( + ( * c0 c2 ) ( + ( * c0 c3 ) ( + ( * c0 c4 ) ( + ( * c0 c5 ) ( + ( * c0 c6 ) ( * c0 c7 ) ) ) ) ) ) ) c9 ) c11 ) ( + ( * ( + ( * c13 c14 ) ( + ( * c15 c16 ) ( + ( * c17 c18 ) ( + ( * c19 c20 ) ( + ( * c21 c22 ) ( + ( * c23 c24 ) ( + ( * c25 c26 ) ( + ( * c27 c28 ) ( + ( * c29 c30 ) ( + ( * c31 c32 ) ( + ( * c33 c34 ) c35 ) ) ) ) ) ) ) ) ) ) ) p36 ) ( + ( + ( * c38 c39 ) ( + ( * c0 c40 ) ( + ( * c0 c41 ) ( + ( * c0 c42 ) ( + ( * c0 c43 ) ( + ( * c0 c44 ) ( + ( * c0 c45 ) ( + ( * c0 c46 ) ( + ( * c0 c47 ) ( + ( * c0 c48 ) ( + ( * c0 c49 ) ( * c0 c50 ) ) ) ) ) ) ) ) ) ) ) ) ( + ( * c0 c52 ) ( + ( * c53 c54 ) ( + ( * c55 c56 ) ( + ( * c0 c57 ) ( + ( * c58 c59 ) ( + ( * c60 c61 ) ( + ( * c62 c63 ) ( + ( * c0 c64 ) ( + ( * c65 c66 ) ( + ( * c67 c68 ) ( + ( * c69 c70 ) ( + ( * c0 c71 ) ( + ( * c72 c73 ) ( + ( * c74 c75 ) ( + ( * c76 c77 ) ( * c0 c78 ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ( * c35 p36 ) ) ( * ( + ( * ( * ( + ( * c1 c14 ) ( + ( * c2 c16 ) ( + ( * c4 c22 ) ( + ( * c5 c24 ) ( + ( * c6 c28 ) ( + ( * c90 c30 ) ( + ( * c7 c32 ) c91 ) ) ) ) ) ) ) p92 ) p36 ) ( + ( * ( + ( * c0 c13 ) ( + ( * c0 c15 ) ( + ( * c0 c17 ) ( + ( * c0 c19 ) ( + ( * c0 c21 ) ( + ( * c0 c23 ) ( + ( * c0 c25 ) ( + ( * c0 c27 ) ( + ( * c0 c29 ) ( + ( * c0 c31 ) ( * c0 c33 ) ) ) ) ) ) ) ) ) ) ) c96 ) ( + ( + ( * c0 c38 ) ( + ( * c40 c54 ) ( + ( * c41 c56 ) ( + ( * c0 c58 ) ( + ( * c0 c60 ) ( + ( * c0 c62 ) ( + ( * c0 c65 ) ( + ( * c0 c67 ) ( + ( * c0 c69 ) ( + ( * c0 c72 ) ( + ( * c0 c74 ) ( * c0 c76 ) ) ) ) ) ) ) ) ) ) ) ) ( + ( * c52 c39 ) ( + ( * c0 c53 ) ( + ( * c0 c55 ) ( + ( * c57 c99 ) ( + ( * c42 c59 ) ( + ( * c43 c61 ) ( + ( * c44 c63 ) ( + ( * c64 c100 ) ( + ( * c45 c66 ) ( + ( * c46 c68 ) ( + ( * c47 c70 ) ( + ( * c71 c101 ) ( + ( * c48 c73 ) ( + ( * c49 c75 ) ( + ( * c50 c77 ) ( * c78 c102 ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ( + ( * c91 p92 ) p36 ) ) )