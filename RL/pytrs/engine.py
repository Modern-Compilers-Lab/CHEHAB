
def apply_rule(expr, rule): 
    return rule.apply(expr)

def rewrite_once(expr, rules):
    """
    Attempt to apply each rule to the expression.
    Returns the transformed expression if any rule is applied, else None.
    """
    for rule in rules:
        new_expr = rule.apply(expr)
        if new_expr is not None:
            print(f"Rule Applied: {rule.name}")
            print(f"Before: {expr}")
            print(f"After:  {new_expr}\n")
            return new_expr
    return None

def rewrite_to_fixpoint(expr, rules, limit=100):
    """
    Apply rewrite rules repeatedly until no more changes occur or a limit is reached.
    """
    current = expr
    for i in range(limit):
        new_expr = rewrite_once(current, rules)
        if new_expr is None:
            break
        current = new_expr
    return current

