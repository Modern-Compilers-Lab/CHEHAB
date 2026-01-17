# util.py  ────────────────────────────────────────────────────────────────────
from typing import Set, Dict, Any, List
import random

from expr    import Expr, Var, Const, Op
from veclang import SCALAR_OPS, VECTOR_OPS

# ---------------------------------------------------------------------------
# variable collection / random assignment helpers
# ---------------------------------------------------------------------------

def collect_variables(expr: Expr) -> Set[str]:
    """Return the set of all Var names that appear in *expr*."""
    vars_found: Set[str] = set()

    def _rec(node: Expr):
        if isinstance(node, Var):
            vars_found.add(node.name)
        elif isinstance(node, Op):
            for a in node.args:
                _rec(a)

    _rec(expr)
    return vars_found


def generate_random_assignments(
    expr: Expr,
    min_val: int = -100,
    max_val: int =  100
) -> Dict[str, int]:
    return {
        v: random.randint(min_val, max_val)
        for v in collect_variables(expr)
    }

# ---------------------------------------------------------------------------
# evaluation
# ---------------------------------------------------------------------------

def evaluate_expr(expr: Expr, env: Dict[str, Any]) -> Any:
    """
    Recursively evaluate an expression under variable assignment *env*.
    Scalars are ints; vectors are Python lists of equal length.
    """
    # leaf nodes ------------------------------------------------------------
    if isinstance(expr, Const):
        return expr.value
    if isinstance(expr, Var):
        try:
            return env[expr.name]
        except KeyError as exc:
            raise ValueError(f"variable '{expr.name}' not in env") from exc

    # Op node ---------------------------------------------------------------
    if not isinstance(expr, Op):
        raise TypeError(f"unknown Expr node {expr}")

    op, args = expr.op, expr.args

    # ───────────────────────────────────────────────────────────────────────
    #  SCALAR operations
    # ───────────────────────────────────────────────────────────────────────
    if op in SCALAR_OPS:
        if op == "+":
            if len(args) < 2:
                raise ValueError("operator '+' needs ≥2 operands")
            vals = [evaluate_expr(a, env) for a in args]

            vecs      = [v for v in vals if isinstance(v, list)]
            scalars   = [v for v in vals if not isinstance(v, list)]

            if vecs:  # vector context
                if not scalars or all(s == 0 for s in scalars):
                    # make sure all vectors same length
                    Ls = {len(v) for v in vecs}
                    if len(Ls) != 1:
                        raise ValueError("mismatching vector lengths in '+'")
                    return [sum(items) for items in zip(*vecs)]
                raise ValueError("cannot add non‑zero scalar to vector")
            return sum(vals)

        if op == "-":
            if len(args) == 1:
                return -evaluate_expr(args[0], env)
            if len(args) == 2:
                return (evaluate_expr(args[0], env)
                        - evaluate_expr(args[1], env))
            # left‑associative n‑ary minus
            acc = evaluate_expr(args[0], env)
            for a in args[1:]:
                acc -= evaluate_expr(a, env)
            return acc

        if op == "*":
            prod = 1
            for a in args:
                prod *= evaluate_expr(a, env)
            return prod

        if op == "Neg":          # unary explicit neg
            if len(args) != 1:
                raise ValueError("Neg expects 1 operand")
            return -evaluate_expr(args[0], env)

        # fall‑through
        raise ValueError(f"unsupported scalar operator '{op}'")

    # ───────────────────────────────────────────────────────────────────────
    #  VECTOR operations   (<< now lives here)
    # ───────────────────────────────────────────────────────────────────────
    if op in VECTOR_OPS:

        # plain container ---------------------------------------------------
        if op == "Vec":
            return [evaluate_expr(a, env) for a in args]

        # left‑rotation -----------------------------------------------------
        if op == "<<":
            if len(args) != 2:
                raise ValueError("'<<' expects exactly 2 operands (vector, int)")

            vec_val = evaluate_expr(args[0], env)   # → list
            k_val   = evaluate_expr(args[1], env)   # → int

            if not isinstance(vec_val, list):
                raise ValueError("first operand of '<<' must evaluate to a vector")
            if not isinstance(k_val, int):
                raise ValueError("second operand of '<<' must evaluate to an int")

            n = len(vec_val)
            if n == 0:
                return vec_val                       # empty vector – nothing to do

            k_norm = k_val % n                       # normalise shift
            # left‑rotate: drop the first k_norm elements and append them at end
            return vec_val[k_norm:] + vec_val[:k_norm]

        # binary vector ops -------------------------------------------------
        if op in {"VecAdd", "VecMinus", "VecMul"}:
            if len(args) != 2:
                raise ValueError(f"{op} expects exactly 2 operands")
            a = evaluate_expr(args[0], env)
            b = evaluate_expr(args[1], env)
            if not (isinstance(a, list) and isinstance(b, list) and len(a) == len(b)):
                raise ValueError(f"{op} operands must be equal‑length vectors")
            if op == "VecAdd":
                return [x + y for x, y in zip(a, b)]
            if op == "VecMinus":
                return [x - y for x, y in zip(a, b)]
            return [x * y for x, y in zip(a, b)]          # VecMul

        # unary vector neg --------------------------------------------------
        if op == "VecNeg":
            if len(args) != 1:
                raise ValueError("VecNeg expects 1 operand")
            v = evaluate_expr(args[0], env)
            if not isinstance(v, list):
                raise ValueError("VecNeg operand must be a vector")
            return [-x for x in v]

        raise ValueError(f"unsupported vector operator '{op}'")

    # ----------------------------------------------------------------------
    raise ValueError(f"unknown operator '{op}'")


# ---------------------------------------------------------------------------
# vector helper wrappers (kept for compatibility)
# ---------------------------------------------------------------------------
def vector_add(a: List[Any], b: List[Any]) -> List[Any]:
    return evaluate_expr(Op("VecAdd", [Const(a) if not isinstance(a, Expr) else a,
                                       Const(b) if not isinstance(b, Expr) else b]), {})

def vector_minus(a: List[Any], b: List[Any]) -> List[Any]:
    return evaluate_expr(Op("VecMinus", [Const(a) if not isinstance(a, Expr) else a,
                                         Const(b) if not isinstance(b, Expr) else b]), {})

def vector_mul(a: List[Any], b: List[Any]) -> List[Any]:
    return evaluate_expr(Op("VecMul", [Const(a) if not isinstance(a, Expr) else a,
                                       Const(b) if not isinstance(b, Expr) else b]), {})

def vector_neg(a: List[Any]) -> List[Any]:
    return evaluate_expr(Op("VecNeg", [Const(a) if not isinstance(a, Expr) else a]), {})
