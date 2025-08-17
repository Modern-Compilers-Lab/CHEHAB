"""Rewrite rule functionality."""

from typing import List, Dict, Union, Tuple, Optional
from collections import deque

from expr import Expr, Const, Var, Op
from pattern import Pattern
from vectorization_analyzer import VectorizationAnalyzer
from util import generate_random_assignments, evaluate_expr

MAX_VECTOR_SIZE = 32

class RewriteRule:
    """
    A single rewrite rule: when LHS pattern matches, replace by RHS template.
    Now supports flexible vectorization and rotation vectorization with binary/unary operations.
    """

    def __init__(self, name: str, lhs: Expr, rhs: Expr, rule_type: str = "normal"):
        self.name = name
        self.rule_type = rule_type
        
        if rule_type == "vectorize":
            # Simple uniform vectorization - extract operator names from Var names
            self.scalar_op = lhs.name if isinstance(lhs, Var) else str(lhs)
            self.vector_op = rhs.name if isinstance(rhs, Var) else str(rhs)
            self.lhs = None
            self.rhs = None
        elif rule_type == "vectorize-flexible":
            # Flexible vectorization - extract operator names from Var names
            scalar_op = lhs.name if isinstance(lhs, Var) else str(lhs)
            self.target_ops = [scalar_op]
            self.vector_op = rhs.name if isinstance(rhs, Var) else str(rhs)
            self.min_count = 2  # minimum matching operations to vectorize
            self.lhs = None
            self.rhs = None
        elif rule_type == "vectorize-rotation":
            # Uniform rotation vectorization
            self.scalar_op = lhs.name if isinstance(lhs, Var) else str(lhs)
            self.vector_op = rhs.name if isinstance(rhs, Var) else str(rhs)
            self.max_vector_size = MAX_VECTOR_SIZE  # size limit for resulting vector
            self.lhs = None
            self.rhs = None
        elif rule_type == "vectorize-rotation-flexible":
            # Flexible rotation vectorization
            scalar_op = lhs.name if isinstance(lhs, Var) else str(lhs)
            self.target_ops = [scalar_op]
            self.vector_op = rhs.name if isinstance(rhs, Var) else str(rhs)
            self.min_count = 2
            self.max_vector_size = MAX_VECTOR_SIZE
            self.lhs = None
            self.rhs = None
        else:
            # Regular rule behavior
            self.lhs = Pattern(lhs)
            self.rhs = rhs
            
        self.rotation_rules = []
        if name not in ["rotation-mul", "rotation-add", "rotation-sub", "rotation-neg"]:
            from rule_parser import parse_rules_from_text
            self.rotation_rules = parse_rules_from_text("""
                                                        Rewrite { name: "rotation-mul", searcher: (VecMul x (<< x a)), applier: (VecMul x (<< x a)) }
                                                        Rewrite { name: "rotation-add", searcher: (VecAdd x (<< x a)), applier: (VecAdd x (<< x a)) }
                                                        Rewrite { name: "rotation-sub", searcher: (VecMinus x (<< x a)), applier: (VecMinus x (<< x a)) }
                                                        """)

    def coverage_progress(self, expr_tree: Expr):
        """
        Returns coverage statistics for *this* rule's search pattern.
        """
        if self.rule_type == "vectorize":
            return VectorizationAnalyzer.vectorize_coverage(expr_tree, self.scalar_op)
        elif self.rule_type == "vectorize-flexible":
            return VectorizationAnalyzer.flexible_vectorize_coverage(expr_tree, self.target_ops, self.min_count)
        elif self.rule_type == "vectorize-rotation":
            return VectorizationAnalyzer.rotation_vectorize_coverage(expr_tree, self.scalar_op, self.max_vector_size)
        elif self.rule_type == "vectorize-rotation-flexible":
            return VectorizationAnalyzer.flexible_rotation_vectorize_coverage(expr_tree, self.target_ops, self.min_count, self.max_vector_size)
        else:
            return VectorizationAnalyzer.analyze_lane_coverages(self.lhs.expr, expr_tree)

    def apply(self, expr: Expr) -> Optional[Expr]:
        """
        If rule matches *expr* at the top level, return transformed Expr;
        else None.
        """
        if self.rule_type == "vectorize":
            return self._apply_vectorize(expr)
        elif self.rule_type == "vectorize-flexible":
            return self._apply_flexible_vectorize(expr)
        elif self.rule_type == "vectorize-rotation":
            return self._apply_rotation_vectorize(expr)
        elif self.rule_type == "vectorize-rotation-flexible":
            return self._apply_flexible_rotation_vectorize(expr)
        else:
            # Regular rule logic
            subst = self.lhs.match(expr)
            if subst is None:
                return None
            result = self._build_rhs(self.rhs, subst)
            return result if result.validate_expression() else None

    def _apply_vectorize(self, expr: Expr) -> Optional[Expr]:
        """Apply uniform vectorization - handles both binary and unary operations."""
        if not (isinstance(expr, Op) and expr.op == "Vec"):
            return None
            
        if len(expr.args) < 2:
            return None
            
        # Check if we're dealing with binary or unary operations
        first_lane = expr.args[0]
        if not isinstance(first_lane, Op) or first_lane.op != self.scalar_op:
            return None
        
        is_unary = len(first_lane.args) == 1
        is_binary = len(first_lane.args) == 2
        
        if not (is_unary or is_binary):
            return None
        
        if is_binary:
            # Binary operations (e.g., subtraction: (- a b))
            left_operands = []
            right_operands = []
            
            for lane in expr.args:
                if not (isinstance(lane, Op) and 
                       lane.op == self.scalar_op and 
                       len(lane.args) == 2):
                    return None
                    
                left_operands.append(lane.args[0])
                right_operands.append(lane.args[1])
            
            left_vec = Op("Vec", left_operands)
            right_vec = Op("Vec", right_operands)
            result = Op(self.vector_op, [left_vec, right_vec])
            
        else:  # is_unary
            # Unary operations (e.g., negation: (- a))
            operands = []
            
            for lane in expr.args:
                if not (isinstance(lane, Op) and 
                       lane.op == self.scalar_op and 
                       len(lane.args) == 1):
                    return None
                    
                operands.append(lane.args[0])
            
            operand_vec = Op("Vec", operands)
            # For unary operations, we only need one operand
            result = Op(self.vector_op, [operand_vec])
        
        return result if result.validate_expression() else None

    def _apply_flexible_vectorize(self, expr: Expr) -> Optional[Expr]:
        """Apply flexible vectorization - handles both binary and unary operations with correct ordering."""
        if not (isinstance(expr, Op) and expr.op == "Vec"):
            return None
            
        if len(expr.args) < 2:
            return None
            
        # Analyze which lanes can be vectorized and determine operation type
        binary_lanes = []
        unary_lanes = []
        binary_left_operands = []
        binary_right_operands = []
        unary_operands = []
        
        for i, lane in enumerate(expr.args):
            if isinstance(lane, Op) and lane.op in self.target_ops:
                if len(lane.args) == 2:
                    binary_lanes.append(i)
                    binary_left_operands.append(lane.args[0])
                    binary_right_operands.append(lane.args[1])
                elif len(lane.args) == 1:
                    unary_lanes.append(i)
                    unary_operands.append(lane.args[0])
        
        total_vectorizable = len(binary_lanes) + len(unary_lanes)
        
        # Need at least min_count vectorizable operations
        if total_vectorizable < self.min_count:
            return None
        
        # Don't match if ALL lanes are vectorizable - let uniform rule handle that
        if total_vectorizable == len(expr.args):
            return None
        
        # Determine identity element and check if operation is non-commutative
        target_op = self.target_ops[0]
        is_non_commutative = target_op in {"-", "/"}
        
        if target_op == "*":
            identity_value = 1
        elif target_op in {"+", "-"}:
            identity_value = 0
        elif target_op == "/":
            identity_value = 1
        else:
            identity_value = 0
        
        # Decide whether to handle as binary or unary based on which is more common
        if len(binary_lanes) >= len(unary_lanes):
            # Handle as binary operations
            left_vec_elements = []
            right_vec_elements = []
            
            binary_idx = 0
            unary_idx = 0
            
            for i in range(len(expr.args)):
                if i in binary_lanes:
                    left_vec_elements.append(binary_left_operands[binary_idx])
                    right_vec_elements.append(binary_right_operands[binary_idx])
                    binary_idx += 1
                elif i in unary_lanes:
                    # Convert unary to binary with identity
                    left_vec_elements.append(unary_operands[unary_idx])
                    right_vec_elements.append(Const(identity_value))
                    unary_idx += 1
                else:
                    # Non-vectorizable expression
                    if is_non_commutative:
                        # For non-commutative ops: put non-matching expr on LEFT, identity on RIGHT
                        # This ensures: non_matching_expr OP identity = non_matching_expr
                        left_vec_elements.append(expr.args[i])
                        right_vec_elements.append(Const(identity_value))
                    else:
                        # For commutative ops: can put identity on either side
                        left_vec_elements.append(Const(identity_value))
                        right_vec_elements.append(expr.args[i])
            
            left_vec = Op("Vec", left_vec_elements)
            right_vec = Op("Vec", right_vec_elements)
            result = Op(self.vector_op, [left_vec, right_vec])
            
        else:
            # Handle as unary operations
            operand_vec_elements = []
            
            binary_idx = 0
            unary_idx = 0
            
            for i in range(len(expr.args)):
                if i in unary_lanes:
                    operand_vec_elements.append(unary_operands[unary_idx])
                    unary_idx += 1
                elif i in binary_lanes:
                    # For binary ops in unary context, we could take first operand
                    # For now, let's take the first operand
                    operand_vec_elements.append(binary_left_operands[binary_idx])
                    binary_idx += 1
                else:
                    # Non-vectorizable expression - wrap in target operation
                    operand_vec_elements.append(expr.args[i])
            
            operand_vec = Op("Vec", operand_vec_elements)
            result = Op(self.vector_op, [operand_vec])
        
        return result if result.validate_expression() else None

    def _apply_rotation_vectorize(self, expr: Expr) -> Optional[Expr]:
        """Apply uniform rotation vectorization - handles both binary and unary operations."""
        if not (isinstance(expr, Op) and expr.op == "Vec"):
            return None
            
        original_size = len(expr.args)
        if original_size < 1:
            return None
            
        # Check size limit
        new_size = original_size * 2
        if new_size > self.max_vector_size:
            return None
            
        # Check if we're dealing with binary or unary operations
        first_lane = expr.args[0]
        if not isinstance(first_lane, Op) or first_lane.op != self.scalar_op:
            return None
        
        is_unary = len(first_lane.args) == 1
        is_binary = len(first_lane.args) == 2
        
        if not (is_unary or is_binary):
            return None
        
        if is_binary:
            # Binary operations - extract both operands
            first_operands = []
            second_operands = []
            
            for lane in expr.args:
                if not (isinstance(lane, Op) and 
                       lane.op == self.scalar_op and 
                       len(lane.args) == 2):
                    return None
                first_operands.append(lane.args[0])
                second_operands.append(lane.args[1])
            
            # Create doubled vector: [first_operands..., second_operands...]
            doubled_vector_elements = first_operands + second_operands
            
        else:  # is_unary
            # Unary operations - duplicate the operands
            operands = []
            
            for lane in expr.args:
                if not (isinstance(lane, Op) and 
                       lane.op == self.scalar_op and 
                       len(lane.args) == 1):
                    return None
                operands.append(lane.args[0])
            
            # Create doubled vector: [operands..., operands...]
            doubled_vector_elements = operands + operands
        
        doubled_vector = Op("Vec", doubled_vector_elements)
        
        # Create rotation with shift = original_size
        shift_amount = Const(original_size)
        rotated_vector = Op("<<", [doubled_vector, shift_amount])
        
        # Create final result
        result = Op(self.vector_op, [doubled_vector, rotated_vector])
        
        return result if result.validate_expression() else None

    def _apply_flexible_rotation_vectorize(self, expr: Expr) -> Optional[Expr]:
        """Apply flexible rotation vectorization - handles both binary and unary operations with correct ordering."""
        if not (isinstance(expr, Op) and expr.op == "Vec"):
            return None
            
        original_size = len(expr.args)
        if original_size < 2:
            return None
            
        # Check size limit
        new_size = original_size * 2
        if new_size > self.max_vector_size:
            return None
            
        # Analyze which lanes can be vectorized
        binary_lanes = []
        unary_lanes = []
        binary_first_operands = []
        binary_second_operands = []
        unary_operands = []
        
        for i, lane in enumerate(expr.args):
            if isinstance(lane, Op) and lane.op in self.target_ops:
                if len(lane.args) == 2:
                    binary_lanes.append(i)
                    binary_first_operands.append(lane.args[0])
                    binary_second_operands.append(lane.args[1])
                elif len(lane.args) == 1:
                    unary_lanes.append(i)
                    unary_operands.append(lane.args[0])
        
        total_vectorizable = len(binary_lanes) + len(unary_lanes)
        
        # Special case: for size-2 vectors, allow 1 matching operation
        # For other sizes, need at least min_count vectorizable operations
        min_required = 1 if original_size == 2 else self.min_count
        if total_vectorizable < min_required:
            return None
        
        # Don't match if ALL lanes are vectorizable - let uniform rotation handle that
        if total_vectorizable == original_size:
            return None
        
        # Determine identity element and check if operation is non-commutative
        target_op = self.target_ops[0]
        is_non_commutative = target_op in {"-", "/"}
        
        if target_op == "*":
            identity_value = 1
        elif target_op in {"+", "-"}:
            identity_value = 0
        elif target_op == "/":
            identity_value = 1
        else:
            identity_value = 0
        
        # For rotation, we'll prioritize binary operations
        first_half = []
        second_half = []
        
        binary_idx = 0
        unary_idx = 0
        
        for i in range(original_size):
            if i in binary_lanes:
                # Binary operation - use both operands
                first_half.append(binary_first_operands[binary_idx])
                second_half.append(binary_second_operands[binary_idx])
                binary_idx += 1
            elif i in unary_lanes:
                # Unary operation - duplicate the operand
                first_half.append(unary_operands[unary_idx])
                second_half.append(unary_operands[unary_idx])
                unary_idx += 1
            else:
                # Non-matching expression
                if is_non_commutative:
                    # For non-commutative ops: put non-matching expr on LEFT, identity on RIGHT
                    # This ensures: non_matching_expr OP identity = non_matching_expr
                    first_half.append(expr.args[i])
                    second_half.append(Const(identity_value))
                else:
                    # For commutative ops: can put identity on either side
                    first_half.append(expr.args[i])
                    second_half.append(Const(identity_value))
        
        # Create doubled vector: [first_half..., second_half...]
        doubled_vector_elements = first_half + second_half
        doubled_vector = Op("Vec", doubled_vector_elements)
        
        # Create rotation with shift = original_size
        shift_amount = Const(original_size)
        rotated_vector = Op("<<", [doubled_vector, shift_amount])
        
        # Create final result
        result = Op(self.vector_op, [doubled_vector, rotated_vector])
        
        return result if result.validate_expression() else None

    def _build_rhs(self, template: Expr, subst: Dict[str, Expr]) -> Expr:
        """Instantiate RHS template using the substitution dict."""
        if isinstance(template, Var):
            return subst[template.name]
        if isinstance(template, Const):
            return Const(template.value)
        if isinstance(template, Op):
            return Op(
                template.op,
                [self._build_rhs(arg, subst) for arg in template.args]
            )
        raise TypeError("unknown Expr in RHS build")

    def find_matching_subexpressions(
        self, expr: Expr
    ) -> List[Tuple[List[int], Expr]]:
        if self.rule_type in ["vectorize", "vectorize-flexible", "vectorize-rotation", "vectorize-rotation-flexible"]:
            return self._find_vectorize_matches(expr)
        else:
            # Regular rules
            matches: List[Tuple[List[int], Expr]] = []
            self._find_matches_recursive(expr, [], matches)
            return [
                (p, m) for p, m in matches
                if self._apply_via_path(expr, p) is not None
            ]

    def _find_vectorize_matches(self, expr: Expr) -> List[Tuple[List[int], Expr]]:
        """Find vectorization matches for all vectorization rule types."""
        matches = []
        
        def _find_recursive(current: Expr, path: List[int]):
            if isinstance(current, Op) and current.op == "Vec":
                if self.rule_type == "vectorize":
                    if self._apply_vectorize(current) is not None:
                        matches.append((path.copy(), current))
                elif self.rule_type == "vectorize-flexible":
                    if self._apply_flexible_vectorize(current) is not None:
                        matches.append((path.copy(), current))
                elif self.rule_type == "vectorize-rotation":
                    if self._apply_rotation_vectorize(current) is not None:
                        matches.append((path.copy(), current))
                elif self.rule_type == "vectorize-rotation-flexible":
                    if self._apply_flexible_rotation_vectorize(current) is not None:
                        matches.append((path.copy(), current))
            
            if isinstance(current, Op):
                rotation = False
                for rule in self.rotation_rules:
                    if rule.lhs.match(current) is not None:
                        rotation = True
                        break
                if not rotation:
                        for i, child in enumerate(current.args):
                            _find_recursive(child, path + [i])
                else:
                    _find_recursive( current.args[0],path + [0])   
        _find_recursive(expr, [])
        return matches

    def _find_matches_recursive(
        self, current: Expr, path: List[int],
        matches: List[Tuple[List[int], Expr]]
    ):
        # Use a deque for BFS
        queue = deque([(path, current)])
        while queue:
            cur_path, node = queue.popleft()

            # check for a match
            if self.lhs.match(node) is not None:
                matches.append((cur_path.copy(), node))

            # enqueue children one level deeper
            if isinstance(node, Op):
                rotation = False
                
                for rule in self.rotation_rules:
                    if rule.lhs.match(node) is not None:
                        rotation = True
                if not rotation:
                    for i, child in enumerate(node.args):
                        queue.append((cur_path + [i], child))
                else:
                    queue.append((cur_path + [0], node.args[0]))

    def _apply_via_path(self, expr: Expr, path: List[int]) -> Optional[Expr]:
        WRAPPER_OPS = {"VecMul", "VecAdd", "VecMinus"}

        def rec(node: Expr, subpath: List[int]) -> Expr:
            # reached the target node – standard replacement
            if not subpath:
                return self.apply(node) or node

            # leaf that cannot hold children
            if not isinstance(node, Op):
                return node

            idx = subpath[0]

            # ────────────────────────────────────────────────────────────
            # Special case: we are stepping into the **first operand** of
            # a wrapper (rotation) op.  After we rewrite that operand we
            # must rebuild the *companion* operand so it references the
            # fresh vector.
            # ────────────────────────────────────────────────────────────
            if node.op in WRAPPER_OPS and idx == 0:
                new_vec  = rec(node.args[0], subpath[1:])   # rewrite left
                other    = node.args[1]
                new_other = other  # default: no change
                # common form  (<< old_vec shift)
                if (isinstance(other, Op)
                        and other.op == "<<" and len(other.args) >= 1):
                    new_other = Op("<<", [new_vec, *other.args[1:]])
                # else:
                #     # generic deep replace if the structure is different
                #     new_other = self._replace_expr(other, node.args[0], new_vec)

                return Op(node.op, [new_vec, new_other])

            # default path – just recurse
            new_args = [
                rec(a, subpath[1:]) if i == idx else a
                for i, a in enumerate(node.args)
            ]
            return Op(node.op, new_args)

        return rec(expr, path)

    @staticmethod
    def _replace_expr(node: Expr, old: Expr, new: Expr) -> Expr:
        """Deep structural substitution: replace every occurrence of *old*
        (pointer-equal **or** structurally equal) by *new* inside *node*."""
        if node is old or node == old:
            return new
        if isinstance(node, Op):
            return Op(node.op, [RewriteRule._replace_expr(a, old, new)
                                for a in node.args])
        return node

    def apply_rule(
        self,
        expr:  Expr,
        match: Optional[Expr] = None,
        path:  Optional[List[int]] = None
    ) -> Expr:
        if path is not None:
            return self._apply_via_path(expr, path)
        if match is not None:
            return self._apply_rule_match(expr, match)
        return self.apply(expr) or expr

    def _apply_rule_match(self, expr: Expr, match: Expr) -> Expr:
        if expr is match:
            return self.apply(expr) or expr
        if isinstance(expr, Op):
            new_args = [self._apply_rule_match(a, match) for a in expr.args]
            return Op(expr.op, new_args)
        return expr

    def apply_rule_check(self, expr: Expr, match: Expr | None = None
                         ) -> Tuple[Expr, bool]:
        subst_expr = self.apply_rule(expr, match)
        val_map    = generate_random_assignments(expr)
        return (
            subst_expr,
            evaluate_expr(expr, val_map) == evaluate_expr(subst_expr, val_map)
        )

    def __repr__(self) -> str:
        if self.rule_type == "vectorize":
            return f"name:{self.name},type:vectorize,scalar_op:{self.scalar_op},vector_op:{self.vector_op}"
        elif self.rule_type == "vectorize-flexible":
            return f"name:{self.name},type:vectorize-flexible,target_ops:{self.target_ops},vector_op:{self.vector_op}"
        elif self.rule_type == "vectorize-rotation":
            return f"name:{self.name},type:vectorize-rotation,scalar_op:{self.scalar_op},vector_op:{self.vector_op},max_size:{self.max_vector_size}"
        elif self.rule_type == "vectorize-rotation-flexible":
            return f"name:{self.name},type:vectorize-rotation-flexible,target_ops:{self.target_ops},vector_op:{self.vector_op},max_size:{self.max_vector_size}"
        else:
            return f"name:{self.name},searcher:{self.lhs.expr},applier:{self.rhs}"