
from expr import Expr, Var, Const, Op
from serializer import expr_to_str
import subprocess




LITERAL = 0
STRUCTURE = 2000
VEC_OP = 1
OP = 1
def operations_cost(expr: Expr) -> int:
    
    if isinstance(expr, (Const, Var)):
        node_cost = LITERAL

    elif isinstance(expr, Op):
        op = expr.op
        visit_all_children = True
        if op in ("+", "Add", "-", "Minus", "*", "Mul"):
            node_cost = OP * 10000
        elif op == "Neg":
            node_cost = OP * 10000
        elif op == "<<":
            node_cost = VEC_OP * 50
        elif op == "Vec":
            node_cost = STRUCTURE
        elif op == "VecAdd":
            second_child = expr.args[1] if len(expr.args) > 1 else None
            node_cost = VEC_OP
            if isinstance(second_child, Op):
                if second_child.op == "<<":
                    node_cost = VEC_OP * 1051
                    visit_all_children = False
        elif op == "VecMinus":
            second_child = expr.args[1] if len(expr.args) > 1 else None
            node_cost = VEC_OP
            if isinstance(second_child, Op):
                if second_child.op == "<<":
                    node_cost = VEC_OP * 1051
                    visit_all_children = False
        elif op == "VecMul":
            second_child = expr.args[1] if len(expr.args) > 1 else None
            node_cost = VEC_OP * 100
            if isinstance(second_child, Op):
                if second_child.op == "<<":
                    node_cost = VEC_OP * 2150
                    visit_all_children = False
        elif op == "VecNeg":
            node_cost = VEC_OP
        else:
            raise ValueError(f"Unknown operator: {op}")

        # 3) Recurse into children
        if visit_all_children:
            for child in expr.args:
                node_cost += operations_cost(child)
        else:
            node_cost += operations_cost(expr.args[0] )  # Only recurse into the first child

    else:
        # any other unexpected node
        node_cost = 0

    return node_cost
# def calculate_cost(expr: Expr, vector_size) -> float:
#     """
#     Calculate the cost of an expression based on multiplicative depth, normal depth, total operations, and expression size.
    
#     :param expr: The expression to evaluate.
#     :param var_values: Variable assignments (not used here but can be integrated if needed).
#     :param weights: A dictionary specifying weights for different cost components.
#                     Example: {'depth': 2.0, 'normal_depth': 1.0, 'operations': 0.5, 'size': 0.1}
#     :return: The calculated cost as a float.
#     """
#     weights = {
#         'multiplicative_depth': 0.55,  # Higher weight to prioritize depth minimization
#         'normal_depth': 0.3,
#         'operations': 0.1,
#         'size': 0.05
#     }
    
#     multiplicative_depth = get_multiplicative_depth(expr)
#     normal_depth = get_normal_depth(expr)
#     total_operations = count_operations(expr)
#     expression_size = count_nodes(expr)
#     print("multiplicative_depth :" ,multiplicative_depth)
#     print("depth :" ,normal_depth)
    
#     # Calculate weighted cost
#     cost = (
#         weights.get('multiplicative_depth', 0.55) * multiplicative_depth +
#         weights.get('normal_depth', 0.3) * normal_depth 
       
#     )
#     BASE_LITERAL_COST = 10
#     BASE_OP_COST = 5

#     return None

def get_multiplicative_depth(expr: Expr) -> int:
    """
    Recursively compute the multiplicative depth of an expression.
    
    :param expr: The expression to traverse.
    :return: The multiplicative depth as an integer.
    """
    if isinstance(expr, Const) or isinstance(expr, Var):
        return 0
    elif isinstance(expr, Op):
        op = expr.op
        if not expr.args :
            return 0
        args = expr.args
        
        # Check if the current operator is a multiplicative operator
        if op in {"*", "VecMul"}:
            # Depth is 1 + maximum depth among arguments
            return 1 + max(get_multiplicative_depth(arg) for arg in args)
        else:
            return max(get_multiplicative_depth(arg) for arg in args)
    else:
        raise ValueError(f"Unknown expression type: {expr}")

def get_normal_depth(expr: Expr) -> int:
    """
    Recursively compute the normal depth of an expression.
    
    :param expr: The expression to traverse.
    :return: The normal depth as an integer.
    """
    if isinstance(expr, Const) or isinstance(expr, Var):
        return 1  # Leaf nodes have depth 1
    elif isinstance(expr, Op):
        args = expr.args
        if not args:
            return 1
        else:
            return 1 + max(get_normal_depth(arg) for arg in args)
    else:
        raise ValueError(f"Unknown expression type: {expr}")

def count_operations(expr: Expr) -> int:
    """
    Recursively count the number of operations in an expression.
    
    :param expr: The expression to traverse.
    :return: The total number of operations as an integer.
    """
    if isinstance(expr, Const) or isinstance(expr, Var):
        return 0
    elif isinstance(expr, Op):
    
        # Count current operation plus operations in arguments
        return 1 + sum(count_operations(arg) for arg in expr.args)
    else:
        raise ValueError(f"Unknown expression type: {expr}")

def count_nodes(expr: Expr) -> int:
    """
    Recursively count the number of nodes in an expression tree.
    
    :param expr: The expression to traverse.
    :return: The total number of nodes as an integer.
    """
    if isinstance(expr, Const) or isinstance(expr, Var):
        return 1
    elif isinstance(expr, Op):
        return 1 + sum(count_nodes(arg) for arg in expr.args)
    else:
        raise ValueError(f"Unknown expression type: {expr}")
    
def rotations_cost(expr: Expr, parent: Expr = None) -> float:
    """
    Mirror the Rust `rotations` method:
     - Base cost 1.0 for a `<<"` node under VecAdd/VecMinus/VecMul
     - Multiply by the numeric constant (number of rotations)
     - Sum that plus the recursive child costs
     - Otherwise recurse with zero base cost
    """
    # Only Ops can be rotation sites
    if isinstance(expr, Op):
        # Check for shift under vector-op parent
        if expr.op == "<<" and isinstance(parent, Op) and parent.op in ("VecAdd", "VecMinus", "VecMul"):
            # Extract number of rotations from the second argument
            if len(expr.args) == 2 and isinstance(expr.args[1], Const):
                num_rotations = expr.args[1].value
                base_cost = 1.0 * num_rotations
                # Sum rotation cost plus children
                children_cost = sum(rotations_cost(child, expr) for child in expr.args)
                return base_cost + children_cost
            # if malformed, fall through to default recursion

        # Default: no base rotation cost, just recurse
        return sum(rotations_cost(child, expr) for child in expr.args)

    # Leaves (Const/Var) contribute zero
    return 0.0
def calculate_cost(expr: Expr,
               w_ops=1.0,
               w_rot=1.0,
               w_depth=1.0,
               w_muldepth=1.0) -> float:
    return (
        w_ops * operations_cost(expr) +
        w_rot * rotations_cost(expr) +
        w_depth * get_normal_depth(expr) +
        w_muldepth * get_multiplicative_depth(expr)
    )
