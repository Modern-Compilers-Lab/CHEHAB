from serializer import expr_to_str, expr_to_tok, VARIABLE_RANGE, node_to_id, CONST_OFFSET, PAREN_CLOSE, PAREN_OPEN, MAX_INT_TOKENS
from expr import Expr,Const,Var,Op
from rules import create_rules,group_rules_from_dict
from cost import calculate_cost, get_multiplicative_depth, get_normal_depth,rotations_cost,operations_cost
from parser import parse_sexpr,tokenize
from veclang import ALL_OPS
from util import evaluate_expr, generate_random_assignments
Expr.__str__ = expr_to_str