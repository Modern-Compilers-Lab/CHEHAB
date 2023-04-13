#include "draw_ir.hpp"
#include <ir_const.hpp>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

using namespace std;

namespace fhecompiler
{
namespace util
{
  void draw_ir(const ir::Program &program, ostream &os)
  {
    auto make_node_label = [&program](const ir::Term *term) -> string {
      if (term->is_operation())
        return term->op_code().str_repr();

      const auto &entry_opt = program.get_tag_table_entry(term->label());
      if (entry_opt.has_value())
      {
        if (term->type() == ir::TermType::scalar && entry_opt->get().type() == ir::TagTableEntryType::constant)
        {
          if (!entry_opt->get().value().has_value())
            throw logic_error("constant scalar term without value");

          const auto &value_var = *entry_opt->get().value();
          return visit(
            ir::overloaded{
              [](const auto &other) -> string { throw logic_error("constant scalar term with vector value"); },
              [](const ir::ScalarValue &scalar_value) -> string {
                return visit(
                  ir::overloaded{[](auto value) -> string {
                    return to_string(value);
                  }},
                  scalar_value);
              }},
            value_var);
        }
        return entry_opt->get().tag();
      }
      return term->label();
    };

    auto make_node_attrs = [&program, &make_node_label](const ir::Term *term) -> string {
      unordered_map<ir::TermType, string> type_to_attrs = {
        {ir::TermType::ciphertext, "style=solid"},
        {ir::TermType::plaintext, "style=dashed"},
        {ir::TermType::scalar, "style=dotted"}};

      unordered_map<ir::TagTableEntryType, string> qualifs_to_attrs = {
        {ir::TagTableEntryType::constant, "color=darkgreen fontcolor=darkgreen"},
        {ir::TagTableEntryType::input, "color=red fontcolor=red"},
        {ir::TagTableEntryType::temp, "color=black fontcolor=black"},
        {ir::TagTableEntryType::output, "color=blue fontcolor=blue"}};

      auto type_attrs_it = type_to_attrs.find(term->type());
      if (type_attrs_it == type_to_attrs.end())
        throw logic_error("formatting for term type not found");

      auto qualif_attrs = qualifs_to_attrs.find(program.get_term_qualifiers(term->label()));
      if (qualif_attrs == qualifs_to_attrs.end())
        throw logic_error("formatting for node qualifiers not found");

      string attrs = "label=\"" + make_node_label(term) + "\"";
      if (type_attrs_it->second.size())
        attrs += " " + type_attrs_it->second;

      if (qualif_attrs->second.size())
        attrs += " " + qualif_attrs->second;
      return attrs;
    };

    os << "digraph \"" << program.name() << "\" {\n";
    os << "node [shape=circle width=1 margin=0]\n";
    os << "edge [dir=back]\n";

    const auto &terms = program.get_top_sorted_terms();
    for (auto term : terms)
    {
      os << term->label() << " [" << make_node_attrs(term) << "]\n";
      if (term->is_operation() == false)
        continue;

      for (auto operand : term->operands())
        os << term->label() << " -> " << operand->label() << '\n';
    }

    string key = R"(subgraph cluster_key {
    graph[label="Key"]
    node [width=0.5]
    edge [dir=forward style=invis]

    const_cipher [label=const color=darkgreen fontcolor=darkgreen style=solid]
    in_cipher [label=input color=red fontcolor=red style=solid]
    tmp_cipher [label=tmp color=black fontcolor=black style=solid]
    out_cipher [label=out color=blue fontcolor=blue style=solid]
    cipher [shape=plain]

    const_plain [label=const color=darkgreen fontcolor=darkgreen style=dashed]
    in_plain [label=in icolor=red fontcolor=red style=dashed]
    tmp_plain [label=tmp color=black fontcolor=black style=dashed]
    out_plain [label=out color=blue fontcolor=blue style=dashed]
    plain [shape=plain]

    const_scalar [label=const color=darkgreen fontcolor=darkgreen style=dotted]
    tmp_scalar [label=tmp color=black fontcolor=black style=dotted]
    scalar [shape=plain]
    
    cipher -> plain
    plain -> scalar
    const_cipher -> const_plain
    in_cipher -> in_plain
    tmp_cipher -> tmp_plain
    out_cipher -> out_plain
    const_plain -> const_scalar
    tmp_plain -> tmp_scalar
})";
    os << key << '\n';
    os << "}\n";
  }
} // namespace util
} // namespace fhecompiler
