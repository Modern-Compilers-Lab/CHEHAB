# veclang.py

# from expr import Op, Const, Var

# List of scalar operators
SCALAR_OPS = {"+", "-", "*", "Neg"}

# List of vector operators
VECTOR_OPS = {"Vec", "VecAdd", "VecMinus", "VecMul", "VecNeg","<<"}

# All operators
ALL_OPS = SCALAR_OPS.union(VECTOR_OPS)

SCALAR_UNARY_OPS = {"-"}              # unary minus
SCALAR_BINARY_OPS = {"+", "-", "*"}   # binary scalar ops
VECTOR_UNARY_OPS = {"VecNeg"}
VECTOR_N_ARY_OPS = {"VecAdd", "VecMinus", "VecMul"}

