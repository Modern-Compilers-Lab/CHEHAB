"""Parser for rewrite rules from text format."""

import re
from typing import List

from expr import Expr, Const, Var
from rewrite_rule import RewriteRule
from parser import parse_sexpr


def parse_rules_from_text(rules_text: str) -> List[RewriteRule]:
    """
    Parse rewrite rules from a multiline string.
    Now supports all vectorization rule types with binary/unary operations.

    Each rule is defined in the following format:
    { name: "rule-name", searcher: (s-expression), applier: (s-expression) }
    { name: "rule-name", type: "vectorize", searcher: "+", applier: "VecAdd" }
    { name: "rule-name", type: "vectorize-flexible", searcher: "+", applier: "VecAdd" }
    { name: "rule-name", type: "vectorize-rotation", searcher: "*", applier: "VecMul" }
    { name: "rule-name", type: "vectorize-rotation-flexible", searcher: "*", applier: "VecMul" }

    :param rules_text: Multiline string containing all rewrite rules.
    :return: List of RewriteRule objects.
    """
    rules = []
    # Regular expression to match each rule block
    rule_pattern = re.compile(
        r'{ name: "(.*?)"(?:, type: "(.*?)")?, searcher: (.*?), applier: (.*?) }',
        re.MULTILINE
    )
    
    matches = rule_pattern.findall(rules_text)
    for match in matches:
        name, rule_type, searcher_str, applier_str = match
        rule_type = rule_type or "normal"  # Default to "normal" if not specified
        
        # For vectorize rules, handle searcher/applier as operator strings
        if rule_type in ["vectorize", "vectorize-flexible", "vectorize-rotation", "vectorize-rotation-flexible"]:
            # Clean the strings (remove quotes if present)
            searcher_clean = searcher_str.strip(' "')
            applier_clean = applier_str.strip(' "')
            
            # Create dummy expressions to pass to RewriteRule constructor
            # The actual operators will be extracted in the constructor
            searcher_expr = Var(searcher_clean)  # Store the operator name
            applier_expr = Var(applier_clean)    # Store the vector op name
        else:
            # Regular rules - parse as expressions
            def parse_expr_str(expr_str):
                if expr_str.startswith('('):
                    return parse_sexpr(expr_str)
                elif expr_str.isdigit() or (expr_str.startswith('-') and expr_str[1:].isdigit()):
                    return Const(int(expr_str))
                else:
                    return Var(expr_str)
            
            searcher_expr = parse_expr_str(searcher_str)
            applier_expr = parse_expr_str(applier_str)
        
        # Create RewriteRule object
        rule = RewriteRule(
            name=name,
            lhs=searcher_expr,
            rhs=applier_expr,
            rule_type=rule_type
        )
        rules.append(rule)
    
    return rules