use egg::*;
use crate::{
    veclang::{ConstantFold, VecLang},
};
use std::collections::HashMap;
use log::debug;

pub fn generate_rules_unstructured_code(
    _optimized_rw: bool,
    _initial_operations: Vec<String>,
    _rules_info: &mut HashMap<String, Vec<String>>,
    _initial_rules: &mut Vec<Rewrite<VecLang, ConstantFold>>,
    rules: &mut Vec<Rewrite<VecLang, ConstantFold>>,
) {

    let max_num_elements = 10;
   
    // generate all possible left hand sides
    for num_ele_index in 3..=max_num_elements {
        let mut lhs_pattern_add = Vec::new();
        let mut lhs_pattern_sub = Vec::new();
        let mut lhs_pattern_mul = Vec::new();

        // Generate the LHS pattern
        for num_elements in 0..(num_ele_index - 1) {
            lhs_pattern_add.push(format!("(+ ?a{} ", num_elements));
            lhs_pattern_sub.push(format!("(- ?a{} ", num_elements));
            lhs_pattern_mul.push(format!("(* ?a{} ", num_elements));
        }
        lhs_pattern_add.push(format!("?a{}", num_ele_index - 1));
        lhs_pattern_sub.push(format!("?a{}", num_ele_index - 1));
        lhs_pattern_mul.push(format!("?a{}", num_ele_index - 1));

        for _ in 0..(num_ele_index - 1) {
            lhs_pattern_add.push(')'.to_string());
            lhs_pattern_sub.push(')'.to_string());
            lhs_pattern_mul.push(')'.to_string());
        }

        let full_lhs_pattern_add_string : String = lhs_pattern_add.concat().parse().unwrap();
        let full_lhs_pattern_sub_string : String = lhs_pattern_sub.concat().parse().unwrap();
        let full_lhs_pattern_mul_string : String = lhs_pattern_mul.concat().parse().unwrap();

        debug!("lhs for add is {:?}", full_lhs_pattern_add_string);
        debug!("lhs for sub is {:?}", full_lhs_pattern_sub_string);
        debug!("lhs for mul is {:?}", full_lhs_pattern_mul_string);

        let full_lhs_pattern_add : Pattern<VecLang> = lhs_pattern_add.concat().parse().unwrap();
        let full_lhs_pattern_sub : Pattern<VecLang> = lhs_pattern_sub.concat().parse().unwrap();
        let full_lhs_pattern_mul : Pattern<VecLang> = lhs_pattern_mul.concat().parse().unwrap();

        // Extract all `?a_i` terms from the LHS pattern
        let elements: Vec<String> = lhs_pattern_add // or lhs_pattern_sub or lhs_pattern_mul
        .iter()
        .filter_map(|s| {
            // Find the starting position of "?a" in the string
            s.find("?a").map(|start| s[start..].split_whitespace().next().unwrap().to_string())
        })
        .collect();
      

        let num_elements = elements.len();
        let half_elements = num_elements / 2;
        let modulo_results = num_elements % 2;

        let min_vect_width = if modulo_results == 0 {
            half_elements
        } else {
            half_elements + 1
        };

        let max_vect_width = num_elements - 1;
        let mut rhs_pattern_add = Vec::new();
        let mut rhs_pattern_sub = Vec::new();
        let mut rhs_pattern_mul = Vec::new();

        for vector_width_index in min_vect_width..=max_vect_width {
            debug!("in this iteration, vector_width_index is {:?}", vector_width_index);
            let mut cpt = 0; // Counter for the current vector position

        rhs_pattern_add.push("(Vec ".to_string());
        rhs_pattern_sub.push("(Vec ".to_string());
        rhs_pattern_mul.push("(Vec ".to_string());

        let mut total_rotations = 0;
        let mut is_first_vector = true;

        for (_index, element) in elements.iter().enumerate() {
            rhs_pattern_add.push(element.to_string());
            rhs_pattern_sub.push(element.to_string());
            rhs_pattern_mul.push(element.to_string());

            cpt += 1;

            if cpt != 1 && is_first_vector {       // there is no rotation for the first element and avoiding calculating the rotations of the second vectors
                total_rotations +=1;
            }
            // Check if the current vector is full
            if cpt == vector_width_index {
                rhs_pattern_add.push(") (Vec ".to_string());
                rhs_pattern_sub.push(") (Vec ".to_string());
                rhs_pattern_mul.push(") (Vec ".to_string());
                cpt = 0;
                is_first_vector = false;
            } else {
                rhs_pattern_add.push(" ".to_string());
                rhs_pattern_sub.push(" ".to_string());
                rhs_pattern_mul.push(" ".to_string());
            }
        }

        let full_vectors = cpt == 0; // Determine whether the vectors are full or partial

        if cpt > 0 {
            for _ in cpt..vector_width_index {
                rhs_pattern_add.push("0".to_string());
                rhs_pattern_add.push(" ".to_string());
                rhs_pattern_sub.push("0".to_string());
                rhs_pattern_sub.push(" ".to_string());
                rhs_pattern_mul.push("0".to_string());
                rhs_pattern_mul.push(" ".to_string());
            }
            rhs_pattern_add.pop();
            rhs_pattern_add.push(")".to_string());
            rhs_pattern_sub.pop();
            rhs_pattern_sub.push(")".to_string());
            rhs_pattern_mul.pop();
            rhs_pattern_mul.push(")".to_string());
            
        } else {
            rhs_pattern_add.pop();
            rhs_pattern_add.push(")".to_string());
            rhs_pattern_sub.pop();
            rhs_pattern_sub.push(")".to_string());
            rhs_pattern_mul.pop();
            rhs_pattern_mul.push(")".to_string());
        }

        // Add VecAddRotF or VecAddRotP based on the vector completeness
        if full_vectors {
            rhs_pattern_add.insert(0, "(VecAddRotF ".to_string());
            rhs_pattern_sub.insert(0, "(VecMinusRotF ".to_string());
            rhs_pattern_mul.insert(0, "(VecMulRotF ".to_string());
        } else {
            rhs_pattern_add.insert(0, "(VecAddRotP ".to_string());
            rhs_pattern_sub.insert(0, "(VecMinusRotP ".to_string());
            rhs_pattern_mul.insert(0, "(VecMulRotP ".to_string());
        }

        // Add the total rotation count
        rhs_pattern_add.push(format!(" {})", total_rotations));
        rhs_pattern_sub.push(format!(" {})", total_rotations));
        rhs_pattern_mul.push(format!(" {})", total_rotations));

        let full_rhs_pattern_add_string: String = rhs_pattern_add.concat();
        let full_rhs_pattern_sub_string: String = rhs_pattern_sub.concat();
        let full_rhs_pattern_mul_string: String = rhs_pattern_mul.concat();

        debug!("rhs for add is {:?}", full_rhs_pattern_add_string);
        debug!("rhs for sub is {:?}", full_rhs_pattern_sub_string);
        debug!("rhs for mul is {:?}", full_rhs_pattern_mul_string);                

        let full_rhs_pattern_add : Pattern<VecLang> = rhs_pattern_add.concat().parse().unwrap();
        let full_rhs_pattern_sub : Pattern<VecLang> = rhs_pattern_sub.concat().parse().unwrap();
        let full_rhs_pattern_mul : Pattern<VecLang> = rhs_pattern_mul.concat().parse().unwrap();

        let rule_name_add = format!("add-vec-{}-{}", num_ele_index, vector_width_index);
        let rule_name_sub = format!("sub-vec-{}-{}", num_ele_index, vector_width_index);
        let rule_name_mul = format!("mul-vec-{}-{}", num_ele_index, vector_width_index);

        rules.push(rewrite!(
            rule_name_add.clone(); {full_lhs_pattern_add.clone()} => {full_rhs_pattern_add.clone()} if are_all_symbols_or_nums(elements.clone())
            )
        );
        rules.push(rewrite!(
            rule_name_sub.clone(); {full_lhs_pattern_sub.clone()} => {full_rhs_pattern_sub.clone()} if are_all_symbols_or_nums(elements.clone())
            )
        );
        rules.push(rewrite!(
            rule_name_mul.clone(); {full_lhs_pattern_mul.clone()} => {full_rhs_pattern_mul.clone()} if are_all_symbols_or_nums(elements.clone())
            )
        );

        rhs_pattern_add = Vec::new();
        rhs_pattern_sub = Vec::new();
        rhs_pattern_mul = Vec::new();


        
        }  
    }        
}


pub fn generate_associativity_and_commutativity_rules(
    _optimized_rw: bool,
    _initial_operations: Vec<String>,
    _rules_info: &mut HashMap<String, Vec<String>>,
    _initial_rules: &mut Vec<Rewrite<VecLang, ConstantFold>>,
    rules: &mut Vec<Rewrite<VecLang, ConstantFold>>,
) {

    let associativity_rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![

        // rewrite!("factor-1"; "(* ?a0 (+ ?b0 ?c0))" => "(+ (* ?a0 ?b0) (* ?a0 ?c0))"),

        rewrite!("add-assoc-3-1"; "(+ (+ ?a0 ?b0) ?c0)" => "(+ ?a0 (+ ?b0 ?c0))"),
        rewrite!("sub-assoc-3-1"; "(- (- ?a0 ?b0) ?c0)" => "(- ?a0 (- ?b0 ?c0))"),
        rewrite!("mul-assoc-3-1"; "(* (* ?a0 ?b0) ?c0)" => "(* ?a0 (* ?b0 ?c0))"),

        rewrite!("add-assoc-1"; "(+ (+ ?a0 ?b0) (+ ?c0 ?d0))" => "(+ ?a0 (+ ?b0 (+ ?c0 ?d0)))"),
        rewrite!("add-assoc-2"; "(+ (+ (+ ?a0 ?b0) ?c0) ?d0)" => "(+ ?a0 (+ ?b0 (+ ?c0 ?d0)))"),
        rewrite!("add-assoc-3"; "(+ ?a0 (+ (+ ?b0 ?c0) ?d0))" => "(+ ?a0 (+ ?b0 (+ ?c0 ?d0)))"),
        rewrite!("add-assoc-4"; "(+ (+ ?a0 (+ ?b0 ?c0)) ?d0)" => "(+ ?a0 (+ ?b0 (+ ?c0 ?d0)))"),
        rewrite!("add-assoc-5"; "(+ ?a0 (+ ?b0 (+ ?c0 ?d0)))" => "(+ ?a0 (+ ?b0 (+ ?c0 ?d0)))"),

        rewrite!("sub-assoc-1"; "(- (- ?a0 ?b0) (- ?c0 ?d0))" => "(- ?a0 (- ?b0 (- ?c0 ?d0)))"),
        rewrite!("sub-assoc-2"; "(- (- (- ?a0 ?b0) ?c0) ?d0)" => "(- ?a0 (- ?b0 (- ?c0 ?d0)))"),
        rewrite!("sub-assoc-3"; "(- ?a0 (- (- ?b0 ?c0) ?d0))" => "(- ?a0 (- ?b0 (- ?c0 ?d0)))"),
        rewrite!("sub-assoc-4"; "(- (- ?a0 (- ?b0 ?c0)) ?d0)" => "(- ?a0 (- ?b0 (- ?c0 ?d0)))"),
        rewrite!("sub-assoc-5"; "(- ?a0 (- ?b0 (- ?c0 ?d0)))" => "(- ?a0 (- ?b0 (- ?c0 ?d0)))"),

        rewrite!("mul-assoc-1"; "(* (* ?a0 ?b0) (* ?c0 ?d0))" => "(* ?a0 (* ?b0 (* ?c0 ?d0)))"),
        rewrite!("mul-assoc-2"; "(* (* (* ?a0 ?b0) ?c0) ?d0)" => "(* ?a0 (* ?b0 (* ?c0 ?d0)))"),
        rewrite!("mul-assoc-3"; "(* ?a0 (* (* ?b0 ?c0) ?d0))" => "(* ?a0 (* ?b0 (* ?c0 ?d0)))"),
        rewrite!("mul-assoc-4"; "(* (* ?a0 (* ?b0 ?c0)) ?d0)" => "(* ?a0 (* ?b0 (* ?c0 ?d0)))"),
        rewrite!("mul-assoc-5"; "(* ?a0 (* ?b0 (* ?c0 ?d0)))" => "(* ?a0 (* ?b0 (* ?c0 ?d0)))"),
    
        rewrite!("mul-split"; "(* ?a0 ?b0)" => "(VecMul (Vec ?a0 0 0) (Vec ?b0 0 0))" if are_all_symbols_or_nums(vec!["?a0".to_string(), "?b0".to_string()])),
        rewrite!("add-split"; "(+ ?a0 ?b0)" => "(VecAdd (Vec ?a0 0 0) (Vec ?b0 0 0))" if are_all_symbols_or_nums(vec!["?a0".to_string(), "?b0".to_string()])),
        rewrite!("min-split"; "(- ?a0 ?b0)" => "(VecMinus (Vec ?a0 0 0) (Vec ?b0 0 0))" if are_all_symbols_or_nums(vec!["?a0".to_string(), "?b0".to_string()])),

        rewrite!("balancing-1" ; "(* ?a0 (* ?b0 (+ ?c0 ?d0)))" => "(* (* ?a0 ?b0) (+ ?c0 ?d0))"),
        rewrite!("balancing-2" ; "(* ?a0 (* ?b0 (* ?c0 ?d0)))" => "(* (* ?a0 ?b0) (* ?c0 ?d0))"),
        rewrite!("balancing-3" ; "(+ ?a0 (+ ?b0 (+ ?c0 ?d0)))" => "(+ (+ ?a0 ?b0) (+ ?c0 ?d0))"),
        rewrite!("balancing-4" ; "(- ?a0 (- ?b0 (- ?c0 ?d0)))" => "(- (- ?a0 ?b0) (- ?c0 ?d0))"),
        rewrite!("balancing-5" ; "(* ?a0 (* ?b0 (* ?c0 ?d0)))" => "(* (* ?a0 ?b0) (* ?c0 ?d0))"),

        ];

        let helping_rules : Vec<Rewrite<VecLang, ConstantFold>> = vec![
            rewrite!("distrib-1"; "(* ?a0 (+ ?b0 ?c0))" => "(+ (* ?a0 ?b0) (* ?a0 ?c0))" if are_all_symbols_or_nums(vec!["?a0".to_string(), "?b0".to_string(), "?c0".to_string()])),
            rewrite!("distrib-2"; "(* ?a0 (- ?b0 ?c0))" => "(- (* ?a0 ?b0) (* ?a0 ?c0))" if are_all_symbols_or_nums(vec!["?a0".to_string(), "?b0".to_string(), "?c0".to_string()])),
            rewrite!("distrib-3"; "(* (+ ?a0 ?b0) (+ ?c0 ?d0))" => "(+ (+ (* ?a0 ?c0) (* ?a0 ?d0)) (+ (* ?b0 ?c0) (* ?b0 ?d0)))" if are_all_symbols_or_nums(vec!["?a0".to_string(), "?b0".to_string(), "?c0".to_string(), "?d0".to_string()])),
            rewrite!("distrib-4"; "(* (- ?a0 ?b0) (- ?c0 ?d0))" => "(+ (- (* ?a0 ?c0) (* ?a0 ?d0)) (- (* ?b0 ?c0) (* ?b0 ?d0)))" if are_all_symbols_or_nums(vec!["?a0".to_string(), "?b0".to_string(), "?c0".to_string(), "?d0".to_string()])),
            rewrite!("distrib-6"; "(* (- ?a0 ?b0) (- ?c0 ?d0))" => "(+ (- (* ?a0 ?c0) (* ?a0 ?d0)) (+ (* ?b0 ?d0) (* ?b0 ?c0)))" if are_all_symbols_or_nums(vec!["?a0".to_string(), "?b0".to_string(), "?c0".to_string(), "?d0".to_string()])),
            rewrite!("distrib-7"; "(* (+ ?a0 ?b0) (- ?c0 ?d0))" => "(+ (- (* ?a0 ?c0) (* ?a0 ?d0)) (- (* ?b0 ?c0) (* ?b0 ?d0)))" if are_all_symbols_or_nums(vec!["?a0".to_string(), "?b0".to_string(), "?c0".to_string(), "?d0".to_string()])),
            rewrite!("distrib-8"; "(* (- ?a0 ?b0) (+ ?c0 ?d0))" => "(+ (+ (* ?a0 ?c0) (* ?a0 ?d0)) (- (* ?b0 ?c0) (* ?b0 ?d0)))" if are_all_symbols_or_nums(vec!["?a0".to_string(), "?b0".to_string(), "?c0".to_string(), "?d0".to_string()])),
            rewrite!("simplif-1"; "(+ (- (+ ?a0 ?b0) ?c0) (- (+ ?d0 ?f0) ?g0))" => "(- (+ (+ ?a0 ?b0) (+ ?d0 ?f0)) (+ ?c0 ?g0))"),
            
        ];

        
    rules.extend(associativity_rules);
    rules.extend(helping_rules);
    
}


fn are_all_symbols_or_nums(vars: Vec<String>) -> impl Fn(&mut EGraph<VecLang, ConstantFold>, Id, &Subst) -> bool {
    move |egraph: &mut EGraph<VecLang, ConstantFold>, _, subst| {
        vars.iter().all(|var| {
            let converted_var = var.as_str();
            let nodes = &egraph[subst[converted_var.parse().unwrap()]].nodes;
            nodes.iter().all(|n| matches!(n, VecLang::Symbol(_) | VecLang::Num(_)))
        })
    }
}