#include<iostream>
#include<map>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <sstream>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <unordered_map>
#include <vector>
#include <functional>
#define MOD_BIT_COUNT_MAX 60 

using namespace std ; 
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
/***************************************************************************/
/***************************************************************************/
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
/**************************************************** 
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