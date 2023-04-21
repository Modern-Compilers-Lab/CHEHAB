#include "fheco/util/draw_ir.hpp"
#include "fheco/ir/term_type.hpp"
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>

using namespace std;

namespace fheco
{
namespace util
{
  void draw_ir(ir::Function &func, ostream &os)
  {
    auto make_node_label = [&func](const ir::Term *term) -> string {
      if (term->is_operation())
        return term->op_code().str_repr();

      if (auto input_info = func.get_input_info(term->id()); input_info)
      {
        string label = input_info->label;
        if (auto output_info = func.get_output_info(term->id()); output_info)
          label += "/" + output_info->label;
        return label;
      }

      if (auto const_value = func.get_const_value(term->id()); const_value)
      {
        if (term->type() == ir::TermType::scalar)
        {
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
            *const_value);
        }
        return "const_ptxt_" + to_string(term->id());
      }

      if (auto output_info = func.get_output_info(term->id()); output_info)
        return output_info->label;

      throw logic_error("temp leaf term");
    };

    auto make_term_attrs = [&func, &make_node_label](const ir::Term *term) -> string {
      unordered_map<ir::TermType, string> type_to_attrs = {
        {ir::TermType::ciphertext, "style=solid"},
        {ir::TermType::plaintext, "style=dashed"},
        {ir::TermType::scalar, "style=dotted"}};

      unordered_map<ir::TermQualif, string> qualifs_to_attrs = {
        {ir::TermQualif::temp, "color=black fontcolor=black"},
        {ir::TermQualif::in, "color=red fontcolor=red"},
        {ir::TermQualif::in_out, "color=red fontcolor=blue"},
        {ir::TermQualif::const_, "color=darkgreen fontcolor=darkgreen"},
        {ir::TermQualif::out, "color=blue fontcolor=blue"}};

      auto type_attrs_it = type_to_attrs.find(term->type());
      if (type_attrs_it == type_to_attrs.end())
        throw logic_error("formatting for term type not found");

      auto qualif_attrs = qualifs_to_attrs.find(func.get_term_qualif(term->id()));
      if (qualif_attrs == qualifs_to_attrs.end())
        throw logic_error("formatting for term qualifiers not found");

      string attrs = "label=\"" + make_node_label(term) + "\"";
      if (type_attrs_it->second.size())
        attrs += " " + type_attrs_it->second;

      if (qualif_attrs->second.size())
        attrs += " " + qualif_attrs->second;
      return attrs;
    };

    os << "digraph \"" << func.name() << "\" {\n";
    os << "node [shape=circle width=1 margin=0]\n";
    os << "edge [dir=back]\n";

    for (auto term : func.get_top_sorted_terms())
    {
      os << term->id() << " [" << make_term_attrs(term) << "]\n";
      if (term->is_operation() == false)
        continue;

      for (auto operand : term->operands())
        os << term->id() << " -> " << operand->id() << '\n';

      // impose operands order
      for (auto it = term->operands().begin(); it != term->operands().end() - 1; ++it)
        os << (*it)->id() << " -> ";
      os << term->operands().back()->id() << " [style=invis]\n";
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
} // namespace fheco
