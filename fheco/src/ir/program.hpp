#pragma once

#include "clear_data_evaluator.hpp"
#include "dag.hpp"
#include "fhecompiler_const.hpp"
#include "ir_const.hpp"
#include "op_code.hpp"
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

namespace fhecompiler
{
namespace ir
{
  class TagTableEntry
  {
  public:
    TagTableEntry() : tag_{}, type_{TagTableEntryType::temp}, value_{} {}

    TagTableEntry(std::string tag, ConstantValue value)
      : tag_{std::move(tag)}, type_{TagTableEntryType::constant}, value_{std::move(value)}
    {}

    TagTableEntry(std::string tag, TagTableEntryType type) : tag_{std::move(tag)}, type_{type}, value_{} {}

    inline void set_tag(std::string tag) { tag_ = std::move(tag); }

    inline void set_type(TagTableEntryType type) { type_ = type; }

    inline void set_value(ConstantValue value) { value_ = std::move(value); }

    inline const std::string &tag() const { return tag_; }

    inline TagTableEntryType type() const { return type_; }

    inline const std::optional<ConstantValue> &value() const { return value_; }

  private:
    std::string tag_;
    TagTableEntryType type_;
    std::optional<ConstantValue> value_;
  };

  class Program
  {
  public:
    Program(std::string name, Scheme scheme, std::size_t vector_size, int bit_width, bool signedness);

    void init_input_term(std::string label, TermType type, std::string tag);

    void init_const_term(std::string label, TermType type, std::string tag, VectorValue value);

    void init_const_term(std::string label, TermType type, std::string tag, ScalarValue value);

    template <typename TArg1, typename TArg2, typename TDestination>
    void operate_binary(OpCode op_code, const TArg1 &arg1, const TArg2 &arg2, TDestination &destination);

    template <typename TArg, typename TDestination>
    void operate_unary(OpCode op_code, const TArg &arg, TDestination &destination);

    void tag_term(std::string label, std::string tag);

    void set_term_output(std::string label, std::string tag);

    std::optional<std::reference_wrapper<const TagTableEntry>> get_tag_table_entry(const std::string &label) const;

    TagTableEntryType get_term_qualifiers(const std::string &label) const;

    inline const std::string &name() const { return name_; }

    inline const std::vector<Term *> &get_top_sorted_terms() const { return data_flow_->get_top_sorted_terms(); }

    inline std::size_t vector_size() const { return vector_size_; }

  private:
    std::string name_;

    std::unique_ptr<DAG> data_flow_;

    std::unordered_map<std::string, TagTableEntry> tag_table_;

    fhecompiler::Scheme scheme_;

    std::size_t vector_size_;

    int bit_width_;

    bool signedness_;

    utils::ClearDataEvaluator clear_data_evaluator_;
  };
} // namespace ir
} // namespace fhecompiler
