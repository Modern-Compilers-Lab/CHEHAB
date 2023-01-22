#include "draw_ir.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

string node_label(ir::Program *program, const ir::Program::Ptr &node)
{
  vector<string> opcodes{"undefined", "=",        "encrypt", "+",      "+", "*", "*",         "-",     "-",
                         "rot",       "rot_rows", "rot_col", "square", "-", "^", "modswitch", "relin", "rescale"};
  string label;
  if (node->get_opcode() == ir::OpCode::undefined)
  {
    if (program->get_entry_form_constants_table(node->get_label()).has_value())
      label = (*program->get_entry_form_constants_table(node->get_label())).get().get_entry_value().tag;
    else
      label = node->get_label();
  }
  else
    label = opcodes[static_cast<int>(node->get_opcode())];
  return label;
}

string node_attes(
  ir::Program *program, const ir::Program::Ptr &node, unordered_map<ir::TermType, string> format_by_termtype,
  unordered_map<ir::ConstantTableEntryType, string> format_by_vartype)
{
  string attrs = "label=\"" + node_label(program, node) + "\"";
  if (format_by_termtype[node->get_term_type()].size())
    attrs += " " + format_by_termtype[node->get_term_type()];
  if (format_by_vartype[program->type_of(node->get_label())].size())
    attrs += " " + format_by_vartype[program->type_of(node->get_label())];
  return attrs;
}

void utils::draw_ir(ir::Program *program, std::string ofile_name)
{
  ofstream ofile;
  ofile.open(ofile_name);
  ofile << "digraph \"" << program->get_program_tag() << "\" {" << endl;
  ofile << "node [shape=circle width=1 margin=0]" << endl;
  ofile << "edge [dir=back]" << endl;

  unordered_map<ir::TermType, string> format_by_termtype = {
    {ir::TermType::ciphertextType, "style=dashed"},
    {ir::TermType::plaintextType, "style=dotted"},
    {ir::TermType::scalarType, "style=solid"},
    {ir::TermType::rawDataType, "style=solid"}};
  unordered_map<ir::ConstantTableEntryType, string> format_by_vartype = {
    {ir::ConstantTableEntryType::input, "color=blue fontcolor=blue"},
    {ir::ConstantTableEntryType::output, "color=red fontcolor=red"},
    {ir::ConstantTableEntryType::constant, "color=darkgreen fontcolor=darkgreen"},
    {ir::ConstantTableEntryType::temp, "color=black fontcolor=black"}};

  const vector<ir::Program::Ptr> &nodes = program->get_dataflow_sorted_nodes(true);
  for (const ir::Program::Ptr &node : nodes)
  {
    ofile << node->get_label() << " [" << node_attes(program, node, format_by_termtype, format_by_vartype) << "]"
          << endl;
    for (const ir::Program::Ptr &operand : node->get_operands())
      ofile << node->get_label() << " -> " << operand->get_label() << endl;
  }
  string key = R"(subgraph cluster_key {
    graph[label="Key"]
    node [width=0.5]
    edge [dir=forward style=invis]
    in_cipher [label=input color=blue fontcolor=blue style=dashed]
    tmp_cipher [label=tmp color=black fontcolor=black style=dashed]
    out_cipher [label=out color=red fontcolor=red style=dashed]
    cipher [shape=plain]

    in_plain [label=in icolor=blue fontcolor=blue style=dotted]
    tmp_plain [label=tmp color=black fontcolor=black style=dotted]
    out_plain [label=out color=red fontcolor=red style=dotted]
    plain [shape=plain]
    
    other [color=black fontcolor=black style=solid]
    scalar [color=darkgreen fontcolor=darkgreen style=solid]
    const[shape=plain]
    
    cipher -> plain
    plain -> const
    out_cipher -> out_plain
    in_cipher -> in_plain
    tmp_cipher -> tmp_plain
    in_plain -> other
    out_plain -> scalar
})";
  ofile << key << endl;
  ofile << "}" << endl;
  ofile.close();
}
