#pragma once

#include "fheco/ir/common.hpp"
#include "fheco/ir/term.hpp"
#include "fheco/trs/op_gen_matcher.hpp"
#include "fheco/trs/term_matcher.hpp"
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_map>

namespace fheco::ir
{
class Func;
} // namespace fheco::ir

namespace fheco::util
{
inline constexpr std::string_view terms_key{R"(subgraph cluster_key {
    graph[label="Key"]
    node [width=0.5]
    edge [style=invis]

    op_out_cipher [label=out style=solid color=blue fontcolor=blue]
    op_cipher [label=op style=solid color=black fontcolor=black]
    const_cipher [label=const style=invis color=darkgreen fontcolor=darkgreen]
    in_cipher [label=in style=solid color=red fontcolor=red]
    cipher [shape=plain]

    op_out_plain [label=out style=dashed color=blue fontcolor=blue]
    op_plain [label=op style=dashed color=black fontcolor=black]
    const_plain [label=const style=dashed color=darkgreen fontcolor=darkgreen]
    in_plain [label=in style=dashed color=red fontcolor=red]
    plain [shape=plain]
    
    cipher -> plain
    in_cipher -> in_plain
    const_cipher -> const_plain
    op_cipher -> op_plain
    op_out_cipher -> op_out_plain
}
)"};

inline const std::unordered_map<ir::Term::Type, std::string> term_type_to_attrs = {
  {ir::Term::Type::cipher, "style=solid"}, {ir::Term::Type::plain, "style=dashed"}};

inline const std::unordered_map<ir::TermQualif, std::string> term_qualifs_to_attrs = {
  {ir::TermQualif::in, "color=red fontcolor=red"},
  {ir::TermQualif::in_out, "color=blue fontcolor=red"},
  {ir::TermQualif::const_, "color=darkgreen fontcolor=darkgreen"},
  {ir::TermQualif::const_out, "color=blue fontcolor=darkgreen"},
  {ir::TermQualif::op, "color=black fontcolor=black"},
  {ir::TermQualif::op_out, "color=blue fontcolor=blue"}};

inline constexpr std::string_view term_matchers_key{R"(subgraph cluster_key {
    graph[label="Key"]
    node [width=0.5]
    edge [style=invis]

    const [style=dotted color=red fontcolor=red]
    plain [style=dotted color=blue fontcolor=blue]
    cipher [style=dashed color=black fontcolor=black]
    any [style=solid color=black fontcolor=black]
    term [shape=plain]
}
)"};

inline const std::unordered_map<trs::TermMatcherType, std::string> term_matcher_type_to_attrs = {
  {trs::TermMatcherType::term, "style=solid color=black fontcolor=black"},
  {trs::TermMatcherType::cipher, "style=dashed color=black fontcolor=black"},
  {trs::TermMatcherType::plain, "style=dotted color=blue fontcolor=blue"},
  {trs::TermMatcherType::const_, "style=dotted color=red fontcolor=red"}};

std::string make_term_attrs(const std::shared_ptr<ir::Func> &func, const ir::Term *term, bool id_as_label);

std::string make_term_label(const std::shared_ptr<ir::Func> &func, const ir::Term *term, bool id_as_label);

void draw_term_matcher_util(
  const trs::TermMatcher &term_matcher, std::ostream &os, bool impose_operands_order, int &leaf_occ_id);

void draw_op_gen_matcher_util(
  const trs::OpGenMatcher &op_gen_matcher, std::ostream &os, bool impose_operands_order, int &leaf_occ_id);
} // namespace fheco::util
