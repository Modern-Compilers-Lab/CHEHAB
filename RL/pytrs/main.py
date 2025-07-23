# main.py

from serializer import expr_to_str
from parser import parse_sexpr
from rules import create_rules
from util import evaluate_expr, generate_random_assignments


def main():

    

    rules = create_rules(9)

    
    expr = eval(open("validation.txt",'r').read())

    
    nice = 0
    bad = 0

    print(rules.keys())
    for i in expr  : 
        if i["rule"] in rules : 
            rule = rules[i["rule"]]
            exp = parse_sexpr(i["old"])
            b = rule.find_matching_subexpressions(exp)
            v = generate_random_assignments(exp)
            evals = {' '.join(map(str,evaluate_expr(exp,v)))}
            
            for path,_ in b : 
                test = rule.apply_rule(exp,path=path) 
                evresult = evaluate_expr(test,v)
                
                evals.add(' '.join(map(str,evresult)))


                

                
                v = generate_random_assignments(parse_sexpr(i["old"]))
                evold = evaluate_expr(parse_sexpr(i["old"]),v)
                evresult = evaluate_expr(test,v)
            
                if evresult != evold :
                    bad += 1
                    
                    print(evold)
                    print(evresult)
                    print()
                    with open('bad_logs.txt', 'a') as f:
                        f.write(f'Rule: {rule.name}\n')
                        f.write(f'Input: {i["old"]}\n')
                        f.write(f'Expected: {i["new"]}\n')
                        f.write(f'Got: {expr_to_str(test)}\n')
                        f.write('-' * 50 + '\n')

                else:
                    # print(evold)
                    # print(evresult)
                    # print(evexpected)
                    # print()
                    nice += 1
                    # command = ["./get_rules", "get_matches", str(9), i["old"], i["rule"]]
                    
                    # res = subprocess.run(command, capture_output=True)
                    # a = eval(res.stdout.decode().strip())
                    # if len(a) > 2 :
                    #     print(rule.name)
                    # with open('nice_logs.txt', 'a') as f:
                    #     f.write(f'Rule: {rule.name}\n')
                    #     f.write(f'Input: {i["old"]}\n')
                    #     f.write(f'Expected: {i["new"]}\n')
                    #     f.write(f'Got: {expr_to_string(test)}\n')
                    #     f.write('-' * 50 + '\n')
    print(f'Nice: {nice}')
    print(f'Bad: {bad}')

    
if __name__ == "__main__":
    main()