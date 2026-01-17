#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <limits>
#include <map>
#include <memory>
#include <ostream>
#include <queue>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <sstream>
#include <algorithm>
#include "fheco/ir/op_code.hpp"

using namespace std;

namespace fheco
{
void replace_all(std::string& str, const std::string& from, const std::string& to);

std::queue<std::string>split(const std::string &s);

std::vector<std::string> process_vectorized_code(const string& content);

bool is_literal(const std::string& token);

bool verify_all_vec_elems_eq0(const vector<string>& elems);

bool isSingleOperandExpression(const std::string& expression);

void decompose_vector_op(const vector<string>& vector_elements, vector<string>& vec_ops1 , vector<string>& vec_ops2);

string generate_rotated_expression(string& expression_to_rotate, int number_of_rotations, string operation);

std::vector<std::string> split_string(const std::string& str, char delimiter);

ir::OpCode operationFromString(string operation);

std::vector<int> split_string_ints(const std::string& str, char delimiter);

std::string vectorToString(const std::vector<int>& vec);

string constant_folding(queue<string> &tokens);

string convert_new_ops(queue<string> &tokens);

std::vector<std::string> tokenizeExpression(const std::string& expression);

} // namespace fheco
