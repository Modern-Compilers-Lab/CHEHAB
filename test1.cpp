#include<iostream>
#include <vector>
#include <sstream>
using namespace std ;
int main(){
    string vector_string = "( + op1 op2 )";
    vector_string=vector_string.substr(3);
    vector_string=vector_string.substr(0,vector_string.size()-2);
    vector<string> addition_elements = {"( + op11 op12 )","( + op21 ( + op22 op23 ) )"};
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
    for(int i =0;i<addition_elements.size();i++){
        std::cout<<vec_ops1[i]<<" "<<vec_ops2[i]<<" \n";
    }
}