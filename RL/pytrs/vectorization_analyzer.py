"""Vectorization analysis functionality extracted from pattern matching."""

from typing import List, Tuple
from expr import Expr, Const, Op, Var
from util import generate_random_assignments, evaluate_expr
import re


class VectorizationAnalyzer:
    """Analyzes vectorization opportunities from patterns."""

    @staticmethod
    def analyze_lane_coverages(pattern_expr: Expr, target: Expr) -> List[Tuple[int, int, int]]:
        """
        Scan *target* for every Vec<k> that matches the search width k and
        report, for each such vector:

            (matched, total_lanes, special)

        • matched       – lanes that match the per-lane template
        • total_lanes   – k
        • special       – extra count:
              · for '+' or '-' patterns: lanes where a 0 appears on either side
              · for '*'  patterns:      lanes where a 1 appears on either side
              · otherwise: 0
        """
        if not (isinstance(pattern_expr, Op) and pattern_expr.op == "Vec"):
            raise ValueError("lane_coverages() needs a (Vec …) search pattern")

        k = len(pattern_expr.args)
        lane_pat = VectorizationAnalyzer._get_lanes_template(pattern_expr)
        pat_op = (lane_pat.op if isinstance(lane_pat, Op) else None)
        coverages = []
        assignments = generate_random_assignments(target)

        def _has_literal(node: Expr, literal) -> bool:
            return (isinstance(node.args[0], Const) and node.args[0].value == literal) or \
                   (isinstance(node.args[1], Const) and node.args[1].value == literal)

        def _is_literal_or_eval(node: Expr, lit: int) -> bool:
            if _has_literal(node, lit):
                return True
            try:
                vals = evaluate_expr(node.args[0], assignments)
                vals2 = evaluate_expr(node.args[1], assignments)
                return vals == lit or vals2 == lit
            except Exception as e:
                print(e)
                return False

        def _dfs(node: Expr) -> None:
            if isinstance(node, Op):
                if node.op == "Vec" and len(node.args) == k:
                    matched = 0
                    special = 0
                    for lane in node.args:
                        if VectorizationAnalyzer._match_lane(lane_pat, lane) is not None:
                            matched += 1
                            if isinstance(lane, Op):
                                if pat_op in {"+", "-"} and len(lane.args) > 1 and _is_literal_or_eval(lane, 0):
                                    special += 1
                                elif pat_op == "*" and len(lane.args) > 1 and (_is_literal_or_eval(lane, 1) or _is_literal_or_eval(lane, 0)):
                                    special += 1
                    coverages.append((matched, k, special))
                # recurse into children
                for child in node.args:
                    _dfs(child)

        _dfs(target)
        return coverages

    @staticmethod
    def _get_lanes_template(pattern_expr: Expr) -> Expr:
        """
        Return a *generic* per-lane Pattern obtained from this Vec search
        pattern by stripping trailing digits from every Var name.
        """
        def _strip(node: Expr) -> Expr:
            if isinstance(node, Var):
                return Var(re.sub(r"\d+$", "", node.name))
            if isinstance(node, (Const,)):
                return node
            if isinstance(node, Op):
                return Op(node.op, [_strip(a) for a in node.args])
            raise TypeError("unknown Expr subclass")

        return _strip(pattern_expr.args[0])

    @staticmethod
    def _match_lane(pattern: Expr, target: Expr) -> dict:
        """Simple pattern matching for lane analysis."""
        if isinstance(pattern, Var):
            return {pattern.name: target}
        if isinstance(pattern, Const):
            return {} if (isinstance(target, Const) and target.value == pattern.value) else None
        if isinstance(pattern, Op):
            if not (isinstance(target, Op) and target.op == pattern.op and len(target.args) == len(pattern.args)):
                return None
            subst = {}
            for p_arg, t_arg in zip(pattern.args, target.args):
                result = VectorizationAnalyzer._match_lane(p_arg, t_arg)
                if result is None:
                    return None
                subst.update(result)
            return subst
        return None

    @staticmethod
    def vectorize_coverage(expr_tree: Expr, scalar_op: str) -> List[Tuple[int, int, int]]:
        """Coverage for simple uniform vectorization rules - handles both binary and unary."""
        coverages = []
        
        def _dfs(node: Expr) -> None:
            if isinstance(node, Op):
                if node.op == "Vec" and len(node.args) >= 2:
                    vectorizable = 0
                    special = 0
                    total = len(node.args)
                    
                    for lane in node.args:
                        if isinstance(lane, Op) and lane.op == scalar_op:
                            if len(lane.args) in [1, 2]:
                                vectorizable += 1
                                
                                if scalar_op in {"+", "-"} and len(lane.args) >= 2:
                                    if (isinstance(lane.args[0], Const) and lane.args[0].value == 0) or \
                                       (isinstance(lane.args[1], Const) and lane.args[1].value == 0):
                                        special += 1
                                elif scalar_op == "*" and len(lane.args) >= 2:
                                    if (isinstance(lane.args[0], Const) and lane.args[0].value in {0, 1}) or \
                                       (isinstance(lane.args[1], Const) and lane.args[1].value in {0, 1}):
                                        special += 1
                    
                    if vectorizable == total:
                        coverages.append((vectorizable, total, special))
                
                for child in node.args:
                    _dfs(child)
        
        _dfs(expr_tree)
        return coverages

    @staticmethod
    def flexible_vectorize_coverage(expr_tree: Expr, target_ops: List[str], min_count: int = 2) -> List[Tuple[int, int, int]]:
        """Coverage for flexible vectorization rules - only partial vectorization cases."""
        coverages = []
        
        def _dfs(node: Expr) -> None:
            if isinstance(node, Op):
                if node.op == "Vec" and len(node.args) >= 2:
                    vectorizable = 0
                    special = 0
                    total = len(node.args)
                    
                    for lane in node.args:
                        if isinstance(lane, Op) and lane.op in target_ops:
                            if len(lane.args) in [1, 2]:
                                vectorizable += 1
                                
                                if lane.op in {"+", "-"} and len(lane.args) >= 2:
                                    if (isinstance(lane.args[0], Const) and lane.args[0].value == 0) or \
                                       (isinstance(lane.args[1], Const) and lane.args[1].value == 0):
                                        special += 1
                                elif lane.op == "*" and len(lane.args) >= 2:
                                    if (isinstance(lane.args[0], Const) and lane.args[0].value in {0, 1}) or \
                                       (isinstance(lane.args[1], Const) and lane.args[1].value in {0, 1}):
                                        special += 1
                    
                    if vectorizable >= min_count and vectorizable < total:
                        coverages.append((vectorizable, total, special))
                
                for child in node.args:
                    _dfs(child)
        
        _dfs(expr_tree)
        return coverages

    @staticmethod
    def rotation_vectorize_coverage(expr_tree: Expr, scalar_op: str, max_vector_size: int = 32) -> List[Tuple[int, int, int]]:
        """Coverage for uniform rotation vectorization rules."""
        coverages = []
        
        def _dfs(node: Expr) -> None:
            if isinstance(node, Op):
                if node.op == "Vec" and len(node.args) >= 1:
                    original_size = len(node.args)
                    new_size = original_size * 2
                    
                    if new_size > max_vector_size:
                        return
                    
                    vectorizable = 0
                    special = 0
                    
                    for lane in node.args:
                        if isinstance(lane, Op) and lane.op == scalar_op:
                            if len(lane.args) in [1, 2]:
                                vectorizable += 1
                                
                                if scalar_op in {"+", "-"} and len(lane.args) >= 2:
                                    if (isinstance(lane.args[0], Const) and lane.args[0].value == 0) or \
                                    (isinstance(lane.args[1], Const) and lane.args[1].value == 0):
                                        special += 1
                                elif scalar_op == "*" and len(lane.args) >= 2:
                                    if (isinstance(lane.args[0], Const) and lane.args[0].value in {0, 1}) or \
                                    (isinstance(lane.args[1], Const) and lane.args[1].value in {0, 1}):
                                        special += 1
                    
                    if vectorizable == original_size:
                        coverages.append((vectorizable, original_size, special))
                
                for child in node.args:
                    _dfs(child)
        
        _dfs(expr_tree)
        return coverages

    @staticmethod
    def flexible_rotation_vectorize_coverage(expr_tree: Expr, target_ops: List[str], min_count: int = 2, max_vector_size: int = 32) -> List[Tuple[int, int, int]]:
        """Coverage for flexible rotation vectorization rules."""
        coverages = []
        
        def _dfs(node: Expr) -> None:
            if isinstance(node, Op):
                if node.op == "Vec" and len(node.args) >= 2:
                    original_size = len(node.args)
                    new_size = original_size * 2
                    
                    if new_size > max_vector_size:
                        return
                    
                    vectorizable = 0
                    special = 0
                    
                    for lane in node.args:
                        if isinstance(lane, Op) and lane.op in target_ops:
                            if len(lane.args) in [1, 2]:
                                vectorizable += 1
                                
                                if lane.op in {"+", "-"} and len(lane.args) >= 2:
                                    if (isinstance(lane.args[0], Const) and lane.args[0].value == 0) or \
                                       (isinstance(lane.args[1], Const) and lane.args[1].value == 0):
                                        special += 1
                                elif lane.op == "*" and len(lane.args) >= 2:
                                    if (isinstance(lane.args[0], Const) and lane.args[0].value in {0, 1}) or \
                                       (isinstance(lane.args[1], Const) and lane.args[1].value in {0, 1}):
                                        special += 1
                    
                    min_required = 1 if original_size == 2 else min_count
                    
                    if vectorizable >= min_required and vectorizable < original_size:
                        coverages.append((vectorizable, original_size, special))
                
                for child in node.args:
                    _dfs(child)
        
        _dfs(expr_tree)
        return coverages