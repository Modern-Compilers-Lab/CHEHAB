from pytrs import parse_sexpr,Expr,Const,Var,Op




def remove_duplication(file_path: str,validation_exprs = []):
    unique_expressions = {}
    with open(file_path, "r") as f:
        for line in f:
            exp_str = line.split("|||")[0].strip()
            if not exp_str:
                continue
            try:
                expr = parse_sexpr(exp_str)
            except Exception as e:
                continue
            token_seq = get_token_sequence(exp_str)
            if token_seq not in unique_expressions:
                recap[str(vec_size)] += 1
                unique_expressions[token_seq] = exp_str
    return list(unique_expressions.values())