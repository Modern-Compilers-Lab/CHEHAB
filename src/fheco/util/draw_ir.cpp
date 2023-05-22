#include "fheco/ir/func.hpp"
#include "fheco/ir/term.hpp"
#include "fheco/util/common.hpp"
#include "fheco/util/draw_ir.hpp"
#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

using namespace std;

namespace fheco::util
{
void draw_ir(const shared_ptr<ir::Func> &func, ostream &os, bool id_as_label, bool show_key, bool impose_operands_order)
{
  auto make_node_label = [id_as_label, &func](const ir::Term *term) -> string {
    if (id_as_label)
      return "$" + to_string(term->id());

    // operation term
    if (term->is_operation())
    {
      if (auto output_info = func->data_flow().get_output_info(term); output_info)
        return term->op_code().str_repr() + " (" + output_info->label_ + ")";

      return term->op_code().str_repr();
    }

    // leaf term
    if (auto input_info = func->data_flow().get_input_info(term); input_info)
    {
      string label = input_info->label_;
      if (auto output_info = func->data_flow().get_output_info(term); output_info)
        label += "(" + output_info->label_ + ")";
      return label;
    }
    else if (func->data_flow().is_const(term))
    {
      if (auto output_info = func->data_flow().get_output_info(term); output_info)
        return output_info->label_;

      return "const_$" + to_string(term->id());
    }
    else
      throw logic_error("invalid leaf term, non-input and non-const");
  };

  auto make_term_attrs = [&func, &make_node_label](const ir::Term *term) -> string {
    unordered_map<ir::Term::Type, string> type_to_attrs = {
      {ir::Term::Type::cipher, "style=solid"}, {ir::Term::Type::plain, "style=dashed"}};

    unordered_map<ir::TermQualif, string> qualifs_to_attrs = {
      {ir::TermQualif::in, "color=red fontcolor=red"},
      {ir::TermQualif::in_out, "color=blue fontcolor=red"},
      {ir::TermQualif::const_, "color=darkgreen fontcolor=darkgreen"},
      {ir::TermQualif::const_out, "color=blue fontcolor=darkgreen"},
      {ir::TermQualif::op, "color=black fontcolor=black"},
      {ir::TermQualif::op_out, "color=blue fontcolor=black"}};

    auto type_attrs_it = type_to_attrs.find(term->type());
    if (type_attrs_it == type_to_attrs.end())
      throw logic_error("formatting for term type not found");

    auto qualif_attrs = qualifs_to_attrs.find(func->data_flow().get_qualif(term));
    if (qualif_attrs == qualifs_to_attrs.end())
      throw logic_error("formatting for term qualifiers not found");

    string attrs = "label=\"" + make_node_label(term) + "\"";
    if (type_attrs_it->second.size())
      attrs += " " + type_attrs_it->second;

    if (qualif_attrs->second.size())
      attrs += " " + qualif_attrs->second;
    return attrs;
  };

  os << "digraph \"" << func->name() << "\" {\n";
  os << "node [shape=circle width=1 margin=0]\n";
  os << "edge [dir=back]\n";

  for (auto term : func->get_top_sorted_terms())
  {
    os << term->id() << " [" << make_term_attrs(term) << "]\n";
    if (!term->is_operation())
      continue;

    const auto &operands = term->operands();

    for (auto operand : operands)
      os << term->id() << " -> " << operand->id() << '\n';

    // try to impose operands order
    if (impose_operands_order && operands.size() > 1)
    {
      for (size_t i = 0; i < operands.size() - 1; ++i)
        os << operands[i]->id() << " -> ";
      os << operands.back()->id() << " [style=invis]\n";
    }
  }

  if (show_key)
  {
    const string_view key{R"(subgraph cluster_key {
    graph[label="Key"]
    node [width=0.5]
    edge [dir=forward style=invis]

    op_out_cipher [label=op_out color=blue fontcolor=black style=solid]
    op_cipher [label=op color=black fontcolor=black style=solid]
    const_out_cipher [label=const_out color=blue fontcolor=darkgreen style=invis]
    const_cipher [label=const color=darkgreen fontcolor=darkgreen style=invis]
    in_out_cipher [label=in_out color=blue fontcolor=red style=solid]
    in_cipher [label=in color=red fontcolor=red style=solid]
    cipher [shape=plain]

    op_out_plain [label=op_out color=blue fontcolor=black style=dashed]
    op_plain [label=op color=black fontcolor=black style=dashed]
    const_out_plain [label=const_out color=blue fontcolor=darkgreen style=dashed]
    const_plain [label=const color=darkgreen fontcolor=darkgreen style=dashed]
    in_out_plain [label=in_out color=blue fontcolor=red style=dashed]
    in_plain [label=in color=red fontcolor=red style=dashed]
    plain [shape=plain]
    
    cipher -> plain
    in_cipher -> in_plain
    in_out_cipher -> in_out_plain
    const_cipher -> const_plain
    const_out_cipher -> const_out_plain
    op_cipher -> op_plain
    op_out_cipher -> op_out_plain
})"};
    os << key << '\n';
  }
  os << "}\n";
}
} // namespace fheco::util
