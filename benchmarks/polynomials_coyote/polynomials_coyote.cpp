#include "fheco/fheco.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream> 
#include <random> 
#include <queue>
using namespace std; 
using namespace fheco;
#include "../global_variables.hpp" 

/*************************************************************** */
// Function for generating random numbers
int getRandomNumber(int range) {
    static std::random_device rd;   // Seed generator
    static std::mt19937 rng(rd());  // Random number generator
    std::uniform_int_distribution<int> dist(0, range);
    return dist(rng);
}
/**************************************************************** */
// C++ equivalent of the treeGenerator function
Ciphertext build_polynomial_tree_from_file(queue<string> &tokens,std::unordered_map<std::string , int>& labels_values){
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
            Ciphertext operand1 , operand2;
            if (tokens.front() == "(")
            {
                operand1 = build_polynomial_tree_from_file(tokens,labels_values);
            }
            else
            {
                //std::cout<<"get op1 \n";
                string VarName = tokens.front();
                operand1 = Ciphertext(VarName);
                int number = std::stoi(VarName.substr(1)); 
                labels_values[VarName]=number ;
                tokens.pop();
            }
            if (tokens.front() == "(")
            {
                operand2 = build_polynomial_tree_from_file(tokens,labels_values);
                potential_step += " ";
            }
            else if (tokens.front() != ")")
            {
                //std::cout<<"get op2 \n";
                string VarName = tokens.front();
                operand2 = Ciphertext(VarName);
                int number = std::stoi(VarName.substr(1)); 
                labels_values[VarName]=number ;
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
                if(operationString=="+"){
                    return Ciphertext(operand1)+Ciphertext(operand2);
                }else if(operationString=="*"){
                    return Ciphertext(operand1)*Ciphertext(operand2);
                }
            }
        }
        else
        {
            string VarName = tokens.front();
            int number = std::stoi(VarName.substr(1)); 
            labels_values[VarName]=number ;
            return Ciphertext(VarName);
        }
    }
    throw logic_error("Invalid expression"); 
}
/****************************************************************** */
Ciphertext treeGenerator(int originalDepth, int maxDepth, int& seed, const std::string& regime,std::unordered_map<std::string , int>& labels_values) {
    std::string VarName = "";
    int value = 0 ;
    if (originalDepth == maxDepth || maxDepth == originalDepth - 1) {
        int randomNum = (regime == "100-100") ? getRandomNumber(1) : getRandomNumber(1);
        Ciphertext lhs = treeGenerator(originalDepth, maxDepth - 1, seed, regime,labels_values);
        Ciphertext rhs = treeGenerator(originalDepth, maxDepth - 1, seed, regime,labels_values);
        if (randomNum == 1) {
            return lhs + rhs;
        } else {
            return lhs * rhs;
        }
    }

    if (maxDepth > 0) {
        int randomNum = (regime == "50-50") ? getRandomNumber(3) : (regime == "100-50") ? getRandomNumber(1) : getRandomNumber(0);
        seed += 1;

        if (randomNum > 1) {
            value = getRandomNumber(1024) ; 
            VarName = "x" + std::to_string(seed);
            labels_values[VarName]=value ;
            seed += 1;
            return Ciphertext(VarName);  // Return Ciphertext instance instead of Tree(Var)
        } else {
            Ciphertext lhs = treeGenerator(originalDepth, maxDepth - 1, seed, regime,labels_values);
            Ciphertext rhs = treeGenerator(originalDepth, maxDepth - 1, seed, regime,labels_values);

            if (randomNum == 1) {
                return lhs + rhs;
            } else {
                return lhs * rhs;
            }
        }
    } else {
        value = getRandomNumber(1024) ; 
        VarName = "x" + std::to_string(seed);
        labels_values[VarName]=value ;
        seed += 1;
        return Ciphertext(VarName);  // Return Ciphertext instance instead of Tree(Var)
    }
}
/*****************************************************/
void fhe(int depth,int iteration, string regime) {
    //vector<int> depths = {5, 10};
    //vector<string> regimes = {"50-50", "100-50", "100-100"};
    //int iteration = 1;
    //int depth = depths[0];
    bool construct_polynomial_from_file = true;
    std::unordered_map<std::string, int> labels_values;
    if(construct_polynomial_from_file){
        std::string filename = "polynomial_trees/tree_" + regime + "_" + std::to_string(depth) + ".txt";
        std::ifstream inFile(filename);
        if (!inFile) {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            throw invalid_argument("Unable to open file ");
        }
        string polynomial_arithmetic_circuit = "";
        std::getline(inFile, polynomial_arithmetic_circuit);
        //std::cout<<"===>  Input expression :"<<polynomial_arithmetic_circuit<<" \n";
        queue<string> tokens = fheco::split(polynomial_arithmetic_circuit);
        Ciphertext result = build_polynomial_tree_from_file(tokens,labels_values);
        result.set_output("result");
    }else{
        int seed = 9100 + (iteration - 1) * 100 + (depth*100) + iteration;
        Ciphertext result = treeGenerator(depth, depth, seed, regime,labels_values);
        result.set_output("result");
    }
    // update io file
    string inputs_file_name = "fhe_io_example.txt" ;
    std::ofstream input_file(inputs_file_name);
    /**********************************************************/ 
    string header = "1 "+std::to_string(labels_values.size())+" 1 \n";
    input_file << header;
    string entrie ="" ;
    for(const auto&pair : labels_values){
        entrie=pair.first+" 1 0 "+std::to_string(pair.second)+" \n";
        input_file << entrie;
    }
    // output entrie
    entrie = "result 1 1 \n";
    input_file << entrie;
    input_file.close();
}

/***************************************************/
void print_bool_arg(bool arg, const string &name, ostream &os)
{
  os << (arg ? name : "no_" + name);
}

int main(int argc, char **argv) {
    auto axiomatic = false;
    auto window = 0;
    bool cse = true;
    bool const_folding = true;
    bool call_quantifier = true;
    bool vectorize_code = true;
    /**************************/
    // Argument validation
    if (argc < 5) {
        throw invalid_argument("Some arguments are lacking, needed arguments are: depth, iteration, regime , vectorize_code");
    }

    int optimization_method = 0;  // 0 = egraph (default), 1 = RL
    if (argc > 6)
        optimization_method = stoi(argv[6]);

    int depth = stoi(argv[1]);
    int iteration = stoi(argv[2]);
    string regime = argv[3];
    vectorize_code = stoi(argv[4]);
    
    if (cse) {
        Compiler::enable_cse();
        Compiler::enable_order_operands();
    } else {
        Compiler::disable_cse();
        Compiler::disable_order_operands();
    }

    if (const_folding) {
        Compiler::enable_const_folding();
    } else {
        Compiler::disable_const_folding();
    } 
    /********************************************/
    //Compiler::enable_auto_enc_params_selection();
    chrono::high_resolution_clock::time_point t;
    chrono::duration<double, milli> elapsed;
    t = chrono::high_resolution_clock::now();
    string func_name = "fhe"; 
    size_t slot_count = 1;
    if(vectorize_code){
        const auto &func = Compiler::create_func(func_name, slot_count, 20, false, true);
        fhe(depth, iteration, regime);
        string gen_name = "_gen_he_" + func_name;
        string gen_path = "he/" + gen_name;
        // Creating header and source files 
        ofstream header_os(gen_path + ".hpp");
        if (!header_os) {
            throw logic_error("Failed to create header file");
        }
        ofstream source_os(gen_path + ".cpp");
        if (!source_os) {
            throw logic_error("Failed to create source file");
        }
        cout << " window is " << window << endl;
        Compiler::gen_vectorized_code(func, window, optimization_method);  // add a flag to specify if the benchmark is structured or no
        auto ruleset = Compiler::Ruleset::depth;
        auto rewrite_heuristic = trs::RewriteHeuristic::bottom_up;
        Compiler::compile(func, ruleset, rewrite_heuristic, header_os, gen_name + ".hpp", source_os);
        Compiler::gen_he_code(func, header_os, gen_name + ".hpp", source_os);
        /************/elapsed = chrono::high_resolution_clock::now() - t;
        cout<<"Compile time : \n";
        cout << elapsed.count() << " ms\n";
        if (call_quantifier) 
        {
            util::Quantifier quantifier{func};
            quantifier.run_all_analysis();
            quantifier.print_info(cout);
        }
    }else{
        const auto &func = Compiler::create_func(func_name, slot_count, 20, false, true);
        fhe(depth, iteration, regime);
        std::string updated_inputs_file_name = "fhe_io_example_adapted.txt" ;
        std::string inputs_file_name = "fhe_io_example.txt";
        util::copyFile(inputs_file_name,updated_inputs_file_name);
        string gen_name = "_gen_he_" + func_name;
        string gen_path = "he/" + gen_name;
        // Creating header and source files
        ofstream header_os(gen_path + ".hpp");
        if (!header_os) {
            throw logic_error("Failed to create header file");
        }
        ofstream source_os(gen_path + ".cpp");
        if (!source_os) {
            throw logic_error("Failed to create source file");
        }
        cout << " window is " << window << endl;
        auto ruleset = Compiler::Ruleset::simplification_ruleset;
        auto rewrite_heuristic = trs::RewriteHeuristic::bottom_up;
        Compiler::compile(func, ruleset, rewrite_heuristic, header_os, gen_name + ".hpp", source_os);
        Compiler::gen_he_code(func, header_os, gen_name + ".hpp", source_os);
        /************/elapsed = chrono::high_resolution_clock::now() - t;
        cout<<"Compile time : \n";
        cout << elapsed.count() << " ms\n";
        if (call_quantifier)
        {
            util::Quantifier quantifier{func};
            quantifier.run_all_analysis();
            quantifier.print_info(cout);
        }
    }
    return 0;
}