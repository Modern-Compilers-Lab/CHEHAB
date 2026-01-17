"""Core pattern matching functionality."""

from typing import List, Dict, Union, Tuple, Optional
import re

from expr import Expr, Const, Var, Op
from veclang import SCALAR_OPS, VECTOR_OPS
from util import generate_random_assignments, evaluate_expr


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