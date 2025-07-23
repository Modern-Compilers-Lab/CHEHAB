#!/usr/bin/env python3
# veclang_gen.py  (updated)

import os, sys, random
sys.path.append(os.path.abspath("./pytrs"))

from pytrs import MAX_INT_TOKENS   # upper bound for arbitrary integers
from pytrs import (tokenize)  

# configurable ranges
MAX_INT_NUMBER = MAX_INT_TOKENS          # ± range for random constants
VARIABLE_NAMES = [f"v{i}" for i in range(1, 2_000)]

# operator families
SCALAR_UNARY_OPS   = ["-"]                # unary minus
SCALAR_BINARY_OPS  = ["+", "-", "*"]

VECTOR_UNARY_OPS   = ["VecNeg"]
VECTOR_N_ARY_OPS   = ["VecAdd", "VecMinus", "VecMul"]

ROT_OP             = "<<"                 # (<< <vector> k)

# helpers
def rnd_var()  -> str: return random.choice(VARIABLE_NAMES)
def rnd_int()  -> str: return str(random.randint(-MAX_INT_NUMBER,
                                                 MAX_INT_NUMBER))
def rnd_term() -> str: return random.choice([rnd_var(), rnd_int()])

# scalar expression
def gen_scalar(max_depth: int) -> str:
    if max_depth <= 0:
        return rnd_term()

    kind = random.choices(["unary", "binary", "leaf"], weights=[.2, .5, .3])[0]
    if kind == "unary":
        return f"(- {gen_scalar(max_depth-1)})"

    if kind == "binary":
        op   = random.choice(SCALAR_BINARY_OPS)
        lhs  = gen_scalar(max_depth-1)
        rhs  = gen_scalar(max_depth-1)
        return f"({op} {lhs} {rhs})"

    return rnd_term()

# (Vec ...) container   – exactly vec_len fields,  padded with 0
def gen_vec_container(max_depth: int, vec_len: int) -> str:
    produced = random.randint(1, vec_len)      # how many *real* scalars
    elems = [gen_scalar(max_depth-1) for _ in range(produced)]
    elems.extend("0" for _ in range(vec_len - produced))
    random.shuffle(elems)                      # random zero positions
    return f"(Vec {' '.join(elems)})"

# vector expression  (VecNeg / VecAdd / VecMul / VecMinus / <<)
def gen_vector(max_depth: int, vec_len: int) -> str:
    if max_depth <= 0:
        return gen_vec_container(0, vec_len)

    choice = random.choices(
        ["unary", "nary", "rot", "leaf"],
        weights=[.25, .25, .25, .25]
    )[0]

    # unary VecNeg ------------------------------------------------------------
    if choice == "unary":
        child = random.choice(
            [gen_vec_container(max_depth-1, vec_len),
             gen_vector(max_depth-1, vec_len)]
        )
        return f"(VecNeg {child})"

    # n‑ary  (VecAdd / VecMinus / VecMul)  -----------------------------------
    if choice == "nary":
        op   = random.choice(VECTOR_N_ARY_OPS)
        lhs  = random.choice(
            [gen_vec_container(max_depth-1, vec_len),
             gen_vector(max_depth-1, vec_len)]
        )
        rhs  = random.choice(
            [gen_vec_container(max_depth-1, vec_len),
             gen_vector(max_depth-1, vec_len)]
        )
        return f"({op} {lhs} {rhs})"

    # rotation  (<< vec k) ----------------------------------------------------
    if choice == "rot":
        child = random.choice(
            [gen_vec_container(max_depth-1, vec_len),
             gen_vector(max_depth-1, vec_len)]
        )
        k = random.randint(1, vec_len)        
        return f"(<< {child} {k})"


    return gen_vec_container(max_depth, vec_len)


def generate_expression(max_depth: int, vec_size: int, MAX_SEQ_LEN ) -> str:
    flavour = random.choices(
        ["vec_container", "vector_op", "scalar"],
        weights=[.4, .4, .2]
    )[0]
    
    l = MAX_SEQ_LEN + 1 
    
    while l > MAX_SEQ_LEN :
        if flavour == "vec_container":
            exp = gen_vec_container(max_depth, vec_size)
        if flavour == "vector_op":
            exp = gen_vector(max_depth, vec_size)
        if flavour == "scalar":
            exp = gen_scalar(max_depth)
        
        tok = tokenize(exp)
        
        l = len(tok)

    return exp 



def generate_multiple_expressions(n: int, max_depth: int,
                                  vector_size: int, max_seq) -> list[str]:
    return [generate_expression(max_depth, vector_size, max_seq) for _ in range(n)]


def main():
    random.seed(1337)

    total = 5000000
    depth_vals   = range(1, 16)    
    vec_size_vals= range(1, 33)   
    max_len = 22000
    
    combos      = len(depth_vals) * len(vec_size_vals)
    base_cnt    = total // combos
    remainder   = total % combos

    fn = f"pretraining/dataset_balanced_ROT_{vec_size_vals[-1]}_" \
         f"{depth_vals[-1]}_{total}.txt"

    with open(fn, "w") as fh:
        idx = 0
        for d in depth_vals:
            for v in vec_size_vals:
                count = base_cnt + (1 if idx < remainder else 0)
                exprs = generate_multiple_expressions(count, d, v, max_len)
                random.shuffle(exprs)
                fh.write("\n".join(exprs) + "\n")
                idx += 1


if __name__ == "__main__":
    main()
