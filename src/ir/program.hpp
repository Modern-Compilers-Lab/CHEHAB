#pragma once

#include<string>
#include<memory>
#include"term.hpp"
#include"dag.hpp"
#include<unordered_map>
#include<map>
#include<list>
#include<memory>
#include<variant>
#include"ir_const.hpp"
#include<optional>
#include"fhecompiler_const.hpp"

namespace ir
{

struct ConstantTableEntry
{

  enum ConstantTableEntryType { 
    constant, //nor input nor output
    input, 
    output,
    io //input and output at the same time
  };

  struct Encrypt
  {
    std::string plaintext_label;
    //to be extended in the future maybe
  };

  using ScalarValue = std::variant<int64_t, double>;

  using VectorValue = std::variant<std::vector<int64_t>, std::vector<double>>;

  using EntryValue = std::variant<std::string, ScalarValue, VectorValue, Encrypt>;

  private:

  ConstantTableEntry::ConstantTableEntryType entry_type;
  EntryValue entry_value;

  public:

  ConstantTableEntry() = default;

  ConstantTableEntry(const ConstantTableEntry& entry_copy) = default;
  ConstantTableEntry& operator=(const ConstantTableEntry& entry_copy) = default;

  ConstantTableEntry(ConstantTableEntry&& entry_copy) = default;
  ConstantTableEntry& operator=(ConstantTableEntry& entry_copy) = default;

  ConstantTableEntry(ConstantTableEntry::ConstantTableEntryType _type, EntryValue _value): entry_type(_type), entry_value(_value) {}

  void set_entry_type(ConstantTableEntry::ConstantTableEntryType _type ) { entry_type = _type; }
  
  void set_entry_value(EntryValue _value) { entry_value = _value; }

  EntryValue& get_entry_value() { return entry_value; }

  ConstantTableEntryType get_entry_type() const { return this->entry_type; }

};

class Program
{

  private:
    
  std::string program_tag; // program_tag defines the name of the main evaluation function which will be generated

  std::unique_ptr<DAG> data_flow; //data_flow points to the IR which is a Directed Acyclic Graph (DAG)    

  std::unordered_map<std::string, ConstantTableEntry> constants_table;//we will have a symbol table, the data structure is a hash table
  
  fhecompiler::Scheme program_scheme;

  //BackendLibrary targeted_backend;

  public:
    
  using Ptr = std::shared_ptr<Term>;

  Program(const std::string& tag_value): program_tag{tag_value} 
  {
    data_flow = std::make_unique<DAG>();
  }

  Ptr insert_operation_node(OpCode opcode, const std::vector<Ptr>& operands, std::string label, TermType term_type);

  Ptr find_node_in_data_flow(std::string label ) const;

  void set_symbol_as_output(std::string symbol );

  template < typename T>
  Ptr insert_node(const T& operand)
  {
    auto node_ptr_in_program = find_node_in_data_flow(operand.get_label());
    if(node_ptr_in_program ) 
    {
      return node_ptr_in_program;
    }
    Ptr new_term = std::make_shared<Term>(operand);
    this->data_flow->insert_node(new_term);
    return new_term;
  }

  void set_scheme(fhecompiler::Scheme program_scheme_value) { program_scheme = program_scheme_value; }

  void insert_entry_in_constants_table(std::pair<std::string, ConstantTableEntry> table_entry);

  bool delete_entry_from_constants_table(std::string entry_key);

  bool insert_new_entry_from_existing(std::string new_entry_key, std::string exsisting_entry_key);

  bool insert_new_entry_from_existing_with_delete(std::string new_entry_key, std::string exsisting_entry_key);

  std::optional<std::reference_wrapper<ConstantTableEntry>> get_entry_form_constants_table(const std::string& );

};
  
}// namespace ir