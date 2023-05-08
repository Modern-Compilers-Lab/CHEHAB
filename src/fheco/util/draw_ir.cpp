#include "fheco/util/draw_ir.hpp"
#include "fheco/util/common.hpp"
#include <cstddef>
#include <stdexcept>
#include <string>
#include <unordered_map>

using namespace std;

namespace fheco::util
{
void draw_ir(ir::Func &func, ostream &os)
{
  auto make_node_label = [&func](const ir::Term *term) -> string {
    // operation term
    if (term->is_operation())
    {
      if (auto output_info = func.get_output_info(term->id()); output_info)
        return term->op_code().str_repr() + " (" + output_info->label_ + ")";

      return term->op_code().str_repr();
    }

    // leaf term
    if (auto input_info = func.get_input_info(term->id()); input_info)
    {
      string label = input_info->label_;
      if (auto output_info = func.get_output_info(term->id()); output_info)
        label += "/" + output_info->label_;
      return label;
    }
    else if (auto const_val = func.get_const_val(term->id()); const_val)
      return "const_" + to_string(term->id());
    else
      throw logic_error("temp leaf term");
  };

  auto make_term_attrs = [&func, &make_node_label](const ir::Term *term) -> string {
    unordered_map<ir::TermType, string> type_to_attrs = {
      {ir::TermType::cipher, "style=solid"}, {ir::TermType::plain, "style=dashed"}};

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
    if (!term->is_operation())
      continue;

    const auto &operands = term->operands();

    for (auto operand : operands)
      os << term->id() << " -> " << operand->id() << '\n';

    // impose operands order
    if (operands.size() > 1)
    {
      for (size_t i = 0; i < operands.size() - 1; ++i)
        os << operands[i]->id() << " -> ";
      os << operands.back()->id() << " [style=invis]\n";
    }
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
} // namespace fheco::util
