


from expr import Const, Var, Op
from pattern import RewriteRule
import re
from typing import List
from pattern import RewriteRule,parse_rules_from_text
from expr import Expr, Const, Var
from parser import parse_sexpr
import subprocess




def create_rules(path) :
    #command = ["./pytrs/egraphs-vectorization","rules", str(vector_size)]
    #subprocess.run(command, stderr=subprocess.DEVNULL)
    
    rules_text = open(path,'r').read().replace("?","")
    rules = parse_rules_from_text(rules_text)
    
    rules_dict = {rule.name: rule for rule in rules}
    return rules_dict 



def group_rules_from_dict(rules_dict):
    grouped = {}
    keys = list(rules_dict.keys()) + ["END"]
    for rule_name in keys:
        if rule_name == "END":
            grouped.setdefault("END", []).append(rule_name)
        else:
            m = re.match(r'^(.*?)-(\d+)$', rule_name)
            if m:
                base = m.group(1)
                grouped.setdefault(base, []).append(rule_name)
            else:
                grouped.setdefault(rule_name, []).append(rule_name)
    return grouped