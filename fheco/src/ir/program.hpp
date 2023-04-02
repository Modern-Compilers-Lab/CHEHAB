#pragma once

#include "clear_data_evaluator.hpp"
#include "dag.hpp"
#include "fhecompiler_const.hpp"
#include "ir_const.hpp"
#include "term.hpp"
#include <cstddef>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
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

    std::string get_tag() { return tag; }

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

  ConstantTableEntryType get_entry_type() const { return entry_type; }
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

  std::size_t vector_size;

  int bit_width;

  bool signedness;

  fhecompiler::Scheme scheme;

  double scale = 0.0; // for ckks

  utils::ClearDataEvaluator clear_data_evaluator;

public:
  Program(const std::string &name, int bit_width, bool signedness, std::size_t vector_size, fhecompiler::Scheme scheme)
    : program_tag(name), bit_width(bit_width), signedness(signedness), vector_size(vector_size), scheme(scheme)
  {
    if (bit_width < 11 || bit_width > 60)
      throw std::invalid_argument("bit_width must be in [11, 60]");

    if (vector_size == 0 || (vector_size & (vector_size - 1)) != 0)
      throw std::invalid_argument("vector_size must be a power of two");

    data_flow = std::make_unique<DAG>();
    clear_data_evaluator = utils::ClearDataEvaluator(vector_size, bit_width);
  }

  Term::Ptr insert_operation_node_in_dataflow(
    OpCode _opcode, const std::vector<Term::Ptr> &_operands, std::string label, TermType term_type);

  Term::Ptr find_node_in_dataflow(const std::string &label) const;

  void delete_node_from_dataflow(const std::string &node_label);

  template <typename T>
  Term::Ptr insert_node_in_dataflow(const T &operand)
  {
    auto node_ptr_in_program = find_node_in_dataflow(operand.get_label());
    if (node_ptr_in_program)
    {
      return node_ptr_in_program;
    }
    Term::Ptr new_term = std::make_shared<Term>(operand);
    data_flow->insert_node(new_term, type_of(new_term->get_label()) == ConstantTableEntryType::output);
    return new_term;
  }

  void update_tag_value_in_constants_table_entry(const std::string &entry_key, const std::string &tag_value);

  std::string get_tag_value_in_constants_table_entry(const std::string &entry_key);

  std::optional<std::string> get_tag_value_in_constants_table_entry_if_exists(const std::string &entry_key);

  void set_node_operands(const std::string &node_label, const std::vector<Term::Ptr> &new_opreands);

  void insert_entry_in_constants_table(std::pair<std::string, ConstantTableEntry> table_entry);

  /*
    if is_a_constant is false, then the entry is for a temporary term
  */
  void insert_or_update_entry_in_constants_table(
    const std::string &label, const ConstantValue &constant_value, bool is_a_constant = false);

  void reset_constant_value_value(const std::string &key);

  void set_constant_value_value(const std::string &key, const ConstantValue &value);

  bool delete_entry_from_constants_table(std::string entry_key);

  void set_node_as_output(const std::string &key);

  void delete_node_from_outputs(const std::string &key);

  bool insert_new_entry_from_existing(std::string new_entry_key, std::string exsisting_entry_key);

  bool insert_new_entry_from_existing_with_delete(std::string new_entry_key, std::string exsisting_entry_key);

  ConstantTableEntryType type_of(const std::string &label);

  void compact_assignement(const Term::Ptr &node_ptr);

  void flatten_term_operand_by_one_level_at_index(const Term::Ptr &term, std::size_t index);

  const std::vector<Term::Ptr> &get_dataflow_sorted_nodes(bool clear_existing_order = false) const;

  const std::map<std::string, Term::Ptr> &get_outputs_nodes() const;
  /*
    Search key here is node label. In order to use just call get_label() method of the object you are manipulating
  */
  std::optional<std::reference_wrapper<ConstantTableEntry>> get_entry_form_constants_table(
    const std::string &search_key);

  std::optional<std::reference_wrapper<const ConstantTableEntry>> get_const_entry_form_constants_table(
    const std::string &) const;

  const std::string &get_program_tag() const { return program_tag; }

  int get_bit_width() const { return bit_width; }

  bool get_signedness() const { return signedness; }

  std::size_t get_vector_size() const { return vector_size; }

  fhecompiler::Scheme get_encryption_scheme() const { return scheme; }

  void set_scale(double _scale) { scale = _scale; }

  double get_scale() const { return scale; }

  inline const utils::ClearDataEvaluator &get_clear_data_evaluator() const { return clear_data_evaluator; }

  bool is_tracked_object(const std::string &label);

  void insert_created_node_in_dataflow(const Term::Ptr &node);

  void add_node_to_outputs_nodes(const Term::Ptr &node);

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
  bool update_if_output_entry(const std::string &output_label, const Term::Ptr &node);

  void replace_with(const Term::Ptr &lhs, const Term::Ptr &rhs);

  bool is_output_node(const std::string &label);
};
} // namespace ir
