#include "draw_ir.hpp"
#include <fstream>
#include <map>
#include <stdexcept>
#include <vector>

using namespace std;

string make_node_label(ir::Program *program, const ir::Term::Ptr &node)
{
  vector<string> opcodes{"undefined", "=", "encrypt", "+",          "+",           "*",
                         "*",         "-", "-",       "rotate",     "rotate_rows", "rotate_col",
                         "square",    "-", "^",       "mod_switch", "relin",       "rescale"};
  string label;
  if (node->is_operation_node())
    label = opcodes[static_cast<int>(node->get_opcode())];
  else
  {
    auto const_table_entry_opt = program->get_entry_form_constants_table(node->get_label());
    if (const_table_entry_opt.has_value())
    {
      ir::ConstantTableEntry const_table_entry = *const_table_entry_opt;
      label = const_table_entry.get_entry_value().tag;
    }
    else
      label = node->get_label();
  }
  return label;
}

string make_node_attrs(ir::Program *program, const ir::Term::Ptr &node)
{
  unordered_map<ir::TermType, string> format_by_node_type = {
    {ir::TermType::ciphertext, "style=dashed"},
    {ir::TermType::plaintext, "style=dotted"},
    {ir::TermType::scalar, "style=solid"},
    {ir::TermType::rawData, "style=solid"}};

  unordered_map<ir::ConstantTableEntryType, string> format_by_node_source = {
    {ir::ConstantTableEntryType::input, "color=blue fontcolor=blue"},
    {ir::ConstantTableEntryType::output, "color=red fontcolor=red"},
    {ir::ConstantTableEntryType::constant, "color=darkgreen fontcolor=darkgreen"},
    {ir::ConstantTableEntryType::temp, "color=black fontcolor=black"},
    {ir::ConstantTableEntryType::undefined, ""}};

  auto format_by_node_type_it = format_by_node_type.find(node->get_term_type());
  if (format_by_node_type_it == format_by_node_type.end())
    throw logic_error("formatting for node type not found");

  auto format_by_node_source_it = format_by_node_source.find(program->type_of(node->get_label()));
  if (format_by_node_source_it == format_by_node_source.end())
    throw logic_error("formatting for node source not found");

  string attrs = "label=\"" + make_node_label(program, node) + "\"";

  if (format_by_node_type_it->second.size())
    attrs += " " + format_by_node_type_it->second;

  if (format_by_node_source_it->second.size())
    attrs += " " + format_by_node_source_it->second;
  return attrs;
}

void utils::draw_ir(ir::Program *program, const std::string &output_file)
{
  if (program == nullptr)
    throw invalid_argument("null pointer programtion, programiton is not initialized");

  ofstream ofile;
  ofile.open(output_file);
  ofile << "digraph \"" << program->get_program_tag() << "\" {" << endl;
  ofile << "node [shape=circle width=1 margin=0]" << endl;
  ofile << "edge [dir=back]" << endl;

  const vector<ir::Term::Ptr> &nodes = program->get_dataflow_sorted_nodes(true);
  for (const ir::Term::Ptr &node : nodes)
  {
    ofile << node->get_label() << " [" << make_node_attrs(program, node) << "]" << endl;

    if (node->is_operation_node() == false)
      continue;

    for (const ir::Term::Ptr &operand : node->get_operands())
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
