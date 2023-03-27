#pragma once

#include "dag.hpp"
#include "encryption_parameters.hpp"
#include "fhecompiler_const.hpp"
#include "ir_const.hpp"
#include "term.hpp"
#include <cstddef>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <variant>

namespace ir
{

struct ConstantTableEntry
{
  struct EntryValue
  {

    std::optional<ConstantValue> value;
    std::string tag;

    EntryValue() = default;

    EntryValue(const std::string &_tag, std::optional<ConstantValue> _value) : value(_value), tag(_tag) {}

    EntryValue(const std::string _tag) : tag(_tag), value(std::nullopt) {}

    EntryValue(std::optional<ConstantValue> _value) : value(_value), tag("") {}

    std::string get_tag() { return this->tag; }

    ~EntryValue() {}
  };

  ConstantTableEntryType entry_type;
  EntryValue entry_value;

public:
  ConstantTableEntry() = default;

  ConstantTableEntry(const ConstantTableEntry &entry_copy) = default;
  ConstantTableEntry &operator=(const ConstantTableEntry &entry_copy) = default;

  ConstantTableEntry(ConstantTableEntry &&entry_copy) = default;
  ConstantTableEntry &operator=(ConstantTableEntry &entry_copy) = default;

  ConstantTableEntry(ConstantTableEntryType _type, EntryValue _value) : entry_type(_type), entry_value(_value) {}

  ~ConstantTableEntry() {}

  void set_entry_type(ConstantTableEntryType _type) { entry_type = _type; }

  void set_entry_value(const EntryValue &_value) { entry_value = _value; }

  void set_entry_value(const ConstantValue &_value) { entry_value.value = _value; }

  void set_entry_tag(const std::string &_tag)
  {
    if (_tag.length())
      entry_value.tag = _tag;
  }

  EntryValue &get_entry_value() { return entry_value; }

  const EntryValue &get_const_entry_value() const { return entry_value; }

  ConstantTableEntryType get_entry_type() const { return this->entry_type; }
};

class Program
{

private:
  std::string program_tag; // program_tag defines the name of the main evaluation function which will be generated

  std::unique_ptr<DAG> data_flow; // data_flow points to the IR which is a Directed Acyclic Graph (DAG)

  /*
    std::unordered_map<size_t, std::unique_ptr<DAG>> dags

    in that case we gonna have multiple DAGs, where each dag is associated to an encryption context
    an encryption context is difined by the set of parameters, from the user we need plaintext modulus
  */

  std::unordered_map<std::string, ConstantTableEntry>
    constants_table; // we will have a symbol table, the data structure is a hash table

  int bit_width;

  bool signedness;

  fhecompiler::SecurityLevel sec_level;

  fhecompiler::Scheme scheme;

  double scale = 0.0; // for ckks

  std::size_t vector_size;

  std::size_t number_of_slots;

  fhecompiler::Backend target_backed = fhecompiler::Backend::SEAL;

  std::size_t plain_modulus = 786433;

  // rotation steps in the program, this vector will be empty until rotation keys selection pass
  std::set<int> rotations_keys_steps;

  param_selector::EncryptionParameters params;

  bool uses_mod_switch = false;

public:
  using Ptr = std::shared_ptr<Term>;

  Program() = delete;

  Program(const std::string &tag_value) : program_tag{tag_value} { data_flow = std::make_unique<DAG>(); }

  ~Program() {}

  Ptr insert_operation_node_in_dataflow(
    OpCode _opcode, const std::vector<Ptr> &_operands, std::string label, TermType term_type);

  Ptr find_node_in_dataflow(const std::string &label) const;

  void delete_node_from_dataflow(const std::string &node_label);

  template <typename T>
  Ptr insert_node_in_dataflow(const T &operand)
  {
    auto node_ptr_in_program = find_node_in_dataflow(operand.get_label());
    if (node_ptr_in_program)
    {
      return node_ptr_in_program;
    }
    Ptr new_term = std::make_shared<Term>(operand);
    this->data_flow->insert_node(new_term, this->type_of(new_term->get_label()) == ConstantTableEntryType::output);
    return new_term;
  }

  void set_targeted_backed(fhecompiler::Backend backend) { target_backed = backend; }

  fhecompiler::Backend get_targeted_backend() { return this->target_backed; }

  void set_as_output(const Ptr &node);

  void update_tag_value_in_constants_table_entry(const std::string &entry_key, const std::string &tag_value);

  std::string get_tag_value_in_constants_table_entry(const std::string &entry_key);

  std::optional<std::string> get_tag_value_in_constants_table_entry_if_exists(const std::string &entry_key);

  void set_node_operands(const std::string &node_label, const std::vector<Ptr> &new_opreands);

  void insert_entry_in_constants_table(std::pair<std::string, ConstantTableEntry> table_entry);

  /*
    if is_a_constant is false, then the entry is for a temporary term
  */
  void insert_or_update_entry_in_constants_table(
    const std::string &label, const ir::ConstantValue &constant_value, bool is_a_constant = false);

  void reset_constant_value_value(const std::string &key);

  void set_constant_value_value(const std::string &key, const ir::ConstantValue &value);

  bool delete_entry_from_constants_table(std::string entry_key);

  void delete_node_from_outputs(const std::string &key);

  bool insert_new_entry_from_existing(std::string new_entry_key, std::string exsisting_entry_key);

  bool insert_new_entry_from_existing_with_delete(std::string new_entry_key, std::string exsisting_entry_key);

  ConstantTableEntryType type_of(const std::string &label);

  void compact_assignement(const ir::Term::Ptr &node_ptr);

  void flatten_term_operand_by_one_level_at_index(const ir::Term::Ptr &term, std::size_t index);

  const std::vector<Ptr> &get_dataflow_sorted_nodes(bool clear_existing_order = false) const;

  const std::unordered_map<std::string, Ptr> &get_outputs_nodes() const;
  /*
    Search key here is node label. In order to use just call get_label() method of the object you are manipulating
  */
  std::optional<std::reference_wrapper<ConstantTableEntry>> get_entry_form_constants_table(
    const std::string &search_key);

  std::optional<std::reference_wrapper<const ConstantTableEntry>> get_const_entry_form_constants_table(
    const std::string &) const;

  const std::string &get_program_tag() const { return this->program_tag; }

  fhecompiler::Scheme get_encryption_scheme() const { return this->scheme; }

  void set_bit_width(int bit_width) { this->bit_width = bit_width; }

  int get_bit_width() const { return this->bit_width; }

  void set_signedness(bool signedness) { this->signedness = signedness; }

  bool get_signedness() const { return this->signedness; }

  size_t get_plain_modulus() const { return plain_modulus; }

  void set_vector_size(std::size_t vector_size) { this->vector_size = vector_size; }

  std::size_t get_vector_size() const { return vector_size; }

  void set_sec_level(fhecompiler::SecurityLevel sec_level) { this->sec_level = sec_level; }

  fhecompiler::SecurityLevel get_sec_level() const { return sec_level; }

  void set_scheme(fhecompiler::Scheme scheme) { this->scheme = scheme; }

  fhecompiler::Scheme get_scheme() const { return scheme; }

  void set_scale(double _scale) { scale = _scale; }

  double get_scale() const { return scale; }

  bool is_tracked_object(const std::string &label);

  void insert_created_node_in_dataflow(const Ptr &node);

  void add_node_to_outputs_nodes(const Ptr &node);

  // void set_rotations_steps(std::vector<int32_t> &steps) { rotations_steps = steps; }
  /*
    key is the node label
  */
  std::optional<ConstantValue> get_entry_value_value(const std::string &key) const;

  // const std::vector<int32_t> &get_rotations_steps() const { return rotations_steps; }

  /*
    This method updates the entry in outputs_nodes map in data_flow, the reason is to keep the same identifier
    introduced by the user but change the node associated to this identifier
  */
  bool update_if_output_entry(const std::string &output_label, const Ptr &node);

  void replace_with(const Ptr &lhs, const Ptr &rhs);

  bool is_output_node(const std::string &label);

  void set_rotations_keys_steps(const std::set<int> &steps) { rotations_keys_steps = steps; }

  const std::set<int> &get_rotations_keys_steps() const { return rotations_keys_steps; }

  void set_params(const param_selector::EncryptionParameters &params) { this->params = params; }

  const param_selector::EncryptionParameters &get_params() const { return params; }

  void set_uses_mod_switch(bool uses_mod_switch) { this->uses_mod_switch = uses_mod_switch; }

  bool get_uses_mod_switch() const { return uses_mod_switch; }
};

/*


*/

} // namespace ir
