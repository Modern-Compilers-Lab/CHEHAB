# pattern.py  ────────────────────────────────────────────────────────────────
from typing import List, Dict, Union, Tuple, Optional

from expr    import Expr, Const, Var, Op
from veclang import SCALAR_OPS, VECTOR_OPS   #  << is now in VECTOR_OPS
from util    import generate_random_assignments, evaluate_expr
import re
from collections import deque
from parser import parse_sexpr

class Pattern:
    """
    A pattern is an Expr that may contain pattern‑variables (Var) whose
    concrete sub‑expression is determined when we attempt a match().
    """

    def __init__(self, expr: Expr):
        self.expr = expr
    def lanes_template(self) -> "Pattern":
        """
        Return a *generic* per-lane Pattern obtained from this Vec search
        pattern by stripping trailing digits from every Var name.

        Example:  (Vec (+ ?a0 ?b0) …)  ⟶  Pattern("(+ ?a ?b)")
        """
        if not (isinstance(self.expr, Op) and self.expr.op == "Vec"):
            raise ValueError("lanes_template() requires a (Vec …) search pattern")

        def _strip(node: Expr) -> Expr:
            if isinstance(node, Var):
                return Var(re.sub(r"\d+$", "", node.name))         # drop lane-index
            if isinstance(node, (Const,)):
                return node                                        # constants unchanged
            if isinstance(node, Op):
                return Op(node.op, [_strip(a) for a in node.args]) # recurse
            raise TypeError("unknown Expr subclass")

        # first lane is enough to describe the per-lane structure
        return Pattern(_strip(self.expr.args[0]))

    # ───────────────────────────────────────────────────────────────────────
    # main extension: richer coverage statistics
    # ───────────────────────────────────────────────────────────────────────
    def lane_coverages(self, target: Expr) -> List[Tuple[int, int, int]]:
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
        if not (isinstance(self.expr, Op) and self.expr.op == "Vec"):
            raise ValueError("lane_coverages() needs a (Vec …) search pattern")

        k          = len(self.expr.args)       # vector width
        lane_pat   = self.lanes_template()     # generic per-lane Pattern
        pat_op     = (lane_pat.expr.op
                      if isinstance(lane_pat.expr, Op) else None)
        coverages  = []
        assignments = generate_random_assignments(target)
        # helper to check for the special literal on either side
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
                        if lane_pat.match(lane) is not None:
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
    # ────────────────────────────────────────────────────────────────────────
    # public entry
    # ────────────────────────────────────────────────────────────────────────
    def match(
        self,
        target: Expr,
        debug: bool = False
    ) -> Optional[Dict[str, Expr]]:
        """
        Try to match *self.expr* against *target*.
        Return a substitution dict { pattern‑var‑name ➜ concrete‑Expr } or
        None if matching fails.
        """
        return self._match_expr(
            self.expr,
            target,
            pattern_parent_op=None,
            subst={},
            debug=debug
        )

    # ────────────────────────────────────────────────────────────────────────
    # recursive helper
    # ────────────────────────────────────────────────────────────────────────
    def _match_expr(
        self,
        pattern_expr: Expr,
        target_expr:  Expr,
        pattern_parent_op: Optional[str],
        subst: Dict[str, Expr],
        debug: bool,
        is_second_child:bool=False
    ) -> Optional[Dict[str, Expr]]:

        if debug:
            print(f"[DBG]  trying {pattern_expr}  ≟  {target_expr}")
        # -------------------------------------------------------------------
        # 1)  pattern leaf is a Var  – decide vector vs scalar binding
        # -------------------------------------------------------------------
        if isinstance(pattern_expr, Var):
            # ─ determine if this variable is required to be a vector ───────
            is_vector_var = (
                pattern_parent_op is not None
                and pattern_parent_op != "Vec"
                and pattern_parent_op in VECTOR_OPS
                and pattern_parent_op != "<<"
                
            ) or (
                pattern_parent_op == "<<"
                and not is_second_child
            )
            # ─ check target accordingly ────────────────────────────────────
            if is_vector_var:
                # must be some vector‑producing Op
                if not (isinstance(target_expr, Op) and target_expr.op in VECTOR_OPS):
                    return None
            else:
                # must be a *scalar* expression (Const / Var / scalar‑Op)
                if isinstance(target_expr, (Const, Var)):
                    # if isinstance(target_expr, Var):
                    #     if target_expr.parent_op in VECTOR_OPS:
                    #         return None
                    pass  # ok
                elif isinstance(target_expr, Op) and target_expr.op in SCALAR_OPS:
                    pass  # ok
                else:
                    return None

            # ─ already bound? must be identical ────────────────────────────
            key = pattern_expr.name
            if key in subst:
                return subst if subst[key] == target_expr else None

            # ─ bind for the first time ─────────────────────────────────────
            subst[key] = target_expr
            return subst

        # -------------------------------------------------------------------
        # 2)  pattern leaf is a Const
        # -------------------------------------------------------------------
        if isinstance(pattern_expr, Const):
            return subst if (
                isinstance(target_expr, Const)
                and target_expr.value == pattern_expr.value
            ) else None

        # -------------------------------------------------------------------
        # 3)  pattern internal node is an Op
        # -------------------------------------------------------------------
        if isinstance(pattern_expr, Op):
            if not (
                isinstance(target_expr, Op)
                and target_expr.op == pattern_expr.op
                and len(target_expr.args) == len(pattern_expr.args)
            ):
                return None

            for index,(p_arg, t_arg) in enumerate(zip(pattern_expr.args, target_expr.args)):
                subst = self._match_expr(
                    p_arg, t_arg, pattern_expr.op, subst, debug,index==1
                )
                if subst is None:
                    return None
            return subst

        # -------------------------------------------------------------------
        # 4)  unknown node type
        # -------------------------------------------------------------------
        return None
    def __repr__(self) -> str:
        return f"{self.expr}"


##############################################################################
#  Rewrite rule object
##############################################################################

class RewriteRule:
    """
    A single rewrite rule: when LHS pattern matches, replace by RHS template.
    """

    # ----------------------------------------------------------------------
    def __init__(self, name: str, lhs: Expr, rhs: Expr):
        self.name = name
        self.lhs  = Pattern(lhs)
        self.rhs  = rhs
        self.rotation_rules = []
        if name not in ["rotation-mul", "rotation-add", "rotation-sub", "rotation-neg"]:
            self.rotation_rules = parse_rules_from_text("""
                                                        Rewrite { name: "rotation-mul", searcher: (VecMul x (<< x a)), applier: (VecMul x (<< x a)) }
                                                        Rewrite { name: "rotation-add", searcher: (VecAdd x (<< x a)), applier: (VecAdd x (<< x a)) }
                                                        Rewrite { name: "rotation-sub", searcher: (VecMinus x (<< x a)), applier: (VecMinus x (<< x a)) }
                                                        """)
    def coverage_progress(self, expr_tree: Expr):
        """
        Returns coverage in [0,1] for *this* rule's search pattern.
        """
        matched =self.lhs.lane_coverages(expr_tree)
        return matched 
    # ----------------------------------------------------------------------
    #   high‑level API
    # ----------------------------------------------------------------------
    def apply(self, expr: Expr) -> Optional[Expr]:
        """
        If rule matches *expr* at the top level, return transformed Expr;
        else None.
        """
        subst = self.lhs.match(expr)
        if subst is None:
            return None

        result = self._build_rhs(self.rhs, subst)
        return result if result.validate_expression() else None

    # ----------------------------------------------------------------------
    #   recursive application utilities
    # ----------------------------------------------------------------------
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

    # ………………………………………………………………………………………………………………………………
    # Everything below is unchanged (path‑based rewrite helpers, evaluation
    # check, etc.) – only minor refactoring for type hints.
    # ………………………………………………………………………………………………………………………………

    def find_matching_subexpressions(
        self, expr: Expr
    ) -> List[Tuple[List[int], Expr]]:
        matches: List[Tuple[List[int], Expr]] = []
        self._find_matches_recursive(expr, [], matches)
        # keep only matches that really transform
        return [
            (p, m) for p, m in matches
            if self._apply_via_path(expr, p) is not None
        ]

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

    # ......................................................................
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

    # ......................................................................
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

    # ......................................................................
    def apply_rule_check(self, expr: Expr, match: Expr | None = None
                         ) -> Tuple[Expr, bool]:
        subst_expr = self.apply_rule(expr, match)
        val_map    = generate_random_assignments(expr)
        return (
            subst_expr,
            evaluate_expr(expr, val_map) == evaluate_expr(subst_expr, val_map)
        )
    def __repr__(self) -> str:
        return f"name:{self.name},searcher:{self.lhs.expr},applier:{self.rhs}"


def parse_rules_from_text(rules_text: str) -> List[RewriteRule]:
    """
    Parse rewrite rules from a multiline string.

    Each rule is defined in the following format:
    { name: "rule-name", searcher: (s-expression), applier: (s-expression) }

    :param rules_text: Multiline string containing all rewrite rules.
    :return: List of RewriteRule objects.
    """
    rules = []
    # Regular expression to match each rule block
    rule_pattern = re.compile(
        r'{ name: "(.*?)", searcher: (.*?), applier: (.*?) }',
        re.MULTILINE
    )
    
    matches = rule_pattern.findall(rules_text)
    for match in matches:
        name, searcher_str, applier_str = match
        # Parse searcher
        if searcher_str.startswith('('):
            searcher_expr = parse_sexpr(searcher_str)
        else:
            # Handle single variable or constant
            if searcher_str.isdigit() or (searcher_str.startswith('-') and searcher_str[1:].isdigit()):
                searcher_expr = Const(int(searcher_str))
            else:
                searcher_expr = Var(searcher_str)
        
        # Parse applier
        if applier_str.startswith('('):
            applier_expr = parse_sexpr(applier_str)
        else:
            # Handle single variable or constant
            if applier_str.isdigit() or (applier_str.startswith('-') and applier_str[1:].isdigit()):
                applier_expr = Const(int(applier_str))
            else:
                applier_expr = Var(applier_str)
        
        # Create RewriteRule object
        rule = RewriteRule(
            name=name,
            lhs=searcher_expr,
            rhs=applier_expr
            # condition can be added here if specified in the rule
        )
        rules.append(rule)
    
    return rules
