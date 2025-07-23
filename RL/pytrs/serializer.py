
from expr import Expr, Const, Var, Op

# 0 and 1 are reserved for consts values 
OP_CODES = {
    "+": 4,
    "-": 5,
    "*": 6,
    "Neg": 7,
    "<<": 8,
    "Vec": 9,
    "VecAdd": 10,
    "VecMinus": 11,
    "VecMul": 12,
    "VecNeg": 13
}
PAREN_OPEN = 2
PAREN_CLOSE = 3
VARIABLE_RANGE = (25, 2000)
CONST_OFFSET = VARIABLE_RANGE[1] + 1
MAX_INT_TOKENS = 200


def expr_to_str(expr: Expr) -> str:
    """
    Convert an Expr object into its corresponding S-expression string.
    
    Args:
        expr (Expr): The expression to convert.
        
    Returns:
        str: The S-expression string representation of the expression.
    """
    if isinstance(expr, Const):
        return str(expr.value)
    elif isinstance(expr, Var):
        return expr.name
    elif isinstance(expr, Op):
        # Recursively convert all arguments
        args_str = ' '.join(expr_to_str(arg) for arg in expr.args)
        return f"({expr.op} {args_str})"
    else:
        raise TypeError(f"Unsupported Expr type: {type(expr).__name__}")


def node_to_id(e, var_map,int_map, next_var_id,next_int_id):
    """
    Return (token_id, updated_next_id, node_type)
    
    - token_id: integer ID for this node (operator, variable, or constant)
    - updated_next_id: possibly updated if a new variable was assigned an ID
    - node_type: a string describing the node kind: "op", "var", or "const"
    
    This replaces the old node_to_id(...) function.
    """
    
    if isinstance(e, Const):
        if e.value  in [0,1] :
            token_id = e.value
        else:
                    # If we haven't seen this constant name, assign a new ID 
            if e.value not in int_map:
                int_map[e.value] = next_int_id
                next_int_id += 1
                if next_int_id > CONST_OFFSET+MAX_INT_TOKENS:
                    raise ValueError(f"Too many distinct numbers: exceeded max range {MAX_INT_TOKENS}")

            token_id = int_map[e.value]

        return token_id, next_var_id,next_int_id, "const"

    elif isinstance(e, Var):
        # If we haven't seen this variable name, assign a new ID from next_id
        if e.name not in var_map:
            var_map[e.name] = next_var_id
            next_var_id += 1
            if next_var_id > VARIABLE_RANGE[1]:
                raise ValueError(f"Too many variables: exceeded max range {VARIABLE_RANGE}")

        token_id = var_map[e.name]
        return token_id, next_var_id,next_int_id, "var"

    elif isinstance(e, Op):
        # If it's an operator, just return OP_CODES
        op_code = OP_CODES[e.op] 
        return op_code, next_var_id,next_int_id, "op"

    else:
        raise TypeError(f"Unsupported expression node type: {type(e)}")
    

def expr_to_tok(expr, max_tokens=512):
    """
    Convert an expression tree into a list of integer tokens, incorporating:
      - OP_CODES for operators
      - PAREN_OPEN and PAREN_CLOSE 
      - Variables mapped to integer IDs
      - Constants kept as their integer values
      
    If max_tokens is provided, the function will stop immediately once the 
    token list hits that size, producing a (potentially) partial token stream.
    
    :param expr: The expression tree (Const, Var, or Op).

    :param next_id: The next available integer ID for unseen variables.
    :param max_tokens: An integer token-limit for truncation/early-stop.
    :return: A list of integer tokens (possibly truncated).
    """
    var_map = {}
    next_id = VARIABLE_RANGE[0]
    tokens = []
    depths = []

    def maybe_append_token(token,depth):
        """
        Attempt to append `token` to `tokens`. 
        Return False if we have reached max_tokens, 
        True otherwise (indicating we successfully appended).
        """
        if max_tokens is not None and len(tokens) >= max_tokens:
            return False
        tokens.append(token)
        depths.append(depth)
        return True

    def traverse(e,current_depth=0):
        nonlocal next_id
        
        # If we've already hit max_tokens, short-circuit
        if max_tokens is not None and len(tokens) >= max_tokens:
            return
        
        if isinstance(e, Const):
            # Just append its integer value
            for i in str(e.value):
                ok = maybe_append_token(CONST_OFFSET+int(i),current_depth)
            if not ok:
                return  # Short-circuit if we failed to append
        elif isinstance(e, Var):
            # Map variable name to an integer ID
            if e.name not in var_map:
                var_map[e.name] = next_id
                next_id += 1
                if next_id >= VARIABLE_RANGE[1]:
                    raise ValueError(f"Too many variables: exceeded maximum allowed range of {VARIABLE_RANGE}")
            ok = maybe_append_token(var_map[e.name],current_depth)
            if not ok:
                return
        elif isinstance(e, Op):
            # Opening parenthesis
            ok = maybe_append_token(PAREN_OPEN,current_depth)
            if not ok:
                return
            
            # Operator code (default 999 if unknown)
            op_code = OP_CODES[e.op]
            ok = maybe_append_token(op_code,current_depth)
            if not ok:
                return
            
            # Recursively traverse each argument
            for arg in e.args:
                traverse(arg,current_depth + 1)
                # If we reached the max limit inside recursion, stop
                if max_tokens is not None and len(tokens) >= max_tokens:
                    return
            
            # Closing parenthesis
            maybe_append_token(PAREN_CLOSE,current_depth)
            # Even if we can't append the closing paren, we just stop
        else:
            raise TypeError(f"Unsupported expression node type: {type(e)}")

    # Perform traversal from the root expression
    traverse(expr)
    return tokens, depths


# Const.__str__ = expr_str_method
# Var.__str__ = expr_str_method
# Op.__str__ = expr_str_method
