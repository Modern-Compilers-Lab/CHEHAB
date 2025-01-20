use std::usize;

use crate::{
    veclang::{ConstantFold, VecLang},
};
use egg::rewrite as rw;
use egg::*;
use core::cmp::min;

pub fn ast_depth(expr: &RecExpr<VecLang>) -> usize {
    fn depth_helper(id: Id, expr: &RecExpr<VecLang>) -> usize {
        let node = &expr[id];
        if node.children().is_empty() {
            0 // Leaf node, depth is 1
        } else { 
            // Depth is 1 + max depth of all child nodes
            1 + node.children().iter().map(|&child| depth_helper(child, expr)).max().unwrap_or(0)
        }
    }
    // Start with the root node (usually the last node in `RecExpr`)
    depth_helper(Id::from(expr.as_ref().len() - 1), expr)
}
/************************************/
pub fn get_vector_width(expr: &RecExpr<VecLang>) -> usize {
    fn vector_width_helper(id: Id, expr: &RecExpr<VecLang>) -> usize {
        let node = &expr[id];
        if matches!(node, VecLang::Vec(_)){
            node.children().len()
        }else{
            node.children().iter().map(|&child| vector_width_helper(child, expr)).max().unwrap_or(0)
        }
    }
    // Start with the root node (usually the last node in `RecExpr`)
    vector_width_helper(Id::from(expr.as_ref().len() - 1), expr)
}
/****************************************/
pub fn vector_assoc_min_rules() -> Vec<Rewrite<VecLang, ConstantFold>> {
    let rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![
        rw!("assoc-balan-min-1"; 
        "(VecMinus ?x (VecMinus ?y (VecMinus ?z ?t)))" => 
        "(VecMinus (VecMinus ?x ?y) (VecMinus ?z ?t))"
        if is_vec("?x","?y","?z","?t")
        ),
        rw!("assoc-balan-min-2"; 
        "(VecMinus ?x (VecMinus (VecMinus ?z ?t) ?y))" => 
        "(VecMinus (VecMinus ?x ?z) (VecMinus ?t ?y))"
        if is_vec("?x","?z","?t","?y")
        ),
        rw!("assoc-balan-min-3"; 
        "(VecMinus (VecMinus (VecMinus ?x ?y) ?z) ?t)" => 
        "(VecMinus (VecMinus ?x ?y) (VecMinus ?z ?t))"
        if is_vec("?x","?z","?t","?y")
        ),
        rewrite!("assoc-balan-min-4"; 
        "(VecMinus (VecMinus ?x (VecMinus ?y ?z)) ?t)" => 
        "(VecMinus (VecMinus ?x ?y) (VecMinus ?z ?t))"
        if is_vec("?x","?z","?t","?y")
        ),
    ];
    rules
}
/*******************************************************************/
pub fn vector_assoc_mul_rules() -> Vec<Rewrite<VecLang, ConstantFold>> {
    let rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![
        rw!("assoc-balan-mul-1"; 
        "(VecMul ?x (VecMul ?y (VecMul ?z ?t)))" => 
        "(VecMul (VecMul ?x ?y) (VecMul ?z ?t))"
        if is_vec("?x","?y","?z","?t")
        ),
        rw!("assoc-balan-mul-2"; 
        "(VecMul ?x (VecMul (VecMul ?z ?t) ?y))" => 
        "(VecMul (VecMul ?x ?z) (VecMul ?t ?y))"
        if is_vec("?x","?z","?t","?y")
        ),
        rw!("assoc-balan-mul-3"; 
        "(VecMul (VecMul (VecMul ?x ?y) ?z) ?t)" => 
        "(VecMul (VecMul ?x ?y) (VecMul ?z ?t))"
        if is_vec("?x","?z","?t","?y")
        ),
        rw!("assoc-balan-mul-4"; 
        "(VecMul (VecMul ?x (VecMul ?y ?z)) ?t)" => 
        "(VecMul (VecMul ?x ?y) (VecMul ?z ?t))"
        if is_vec("?x","?z","?t","?y")
        ),
        rw!("assoc-balan-mul-5"; 
        "(VecMul ?x (VecMul (VecMul ?y ?z) ?t))" => 
        "(VecMul (VecMul ?x ?y) (VecMul ?z ?t))"
        if is_vec("?x","?z","?t","?y")
        ),
        rw!("assoc-balan-mul-6"; 
        "(VecMul ?x (VecMul (VecMul ?y ?z) ?t))" => 
        "(VecMul (VecMul ?x ?y) (VecMul ?z ?t))"
        if is_vec("?x","?z","?t","?y")
        ),
    ];
    rules
}
/****************************************/
pub fn vector_assoc_add_rules() -> Vec<Rewrite<VecLang, ConstantFold>> {
    let rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![
        rw!("assoc-balan-add-1"; 
        "(VecAdd ?x (VecAdd ?y (VecAdd ?z ?t)))" => 
        "(VecAdd (VecAdd ?x ?y) (VecAdd ?z ?t))"
        if is_vec("?x","?y","?z","?t")
        ),
        rw!("assoc-balan-add-2"; 
        "(VecAdd ?x (VecAdd (VecAdd ?z ?t) ?y))" => 
        "(VecAdd (VecAdd ?x ?z) (VecAdd ?t ?y))"
        if is_vec("?x","?z","?t","?y")
        ),
        rw!("assoc-balan-add-3"; 
        "(VecAdd (VecAdd (VecAdd ?x ?y) ?z) ?t)" => 
        "(VecAdd (VecAdd ?x ?y) (VecAdd ?z ?t))"
        if is_vec("?x","?z","?t","?y")
        ),
        rw!("assoc-balan-add-4"; 
        "(VecAdd (VecAdd ?x (VecAdd ?y ?z)) ?t)" => 
        "(VecAdd (VecAdd ?x ?y) (VecAdd ?z ?t))"
        if is_vec("?x","?z","?t","?y")
        ),
    ];
    rules 
}
/*******************************************************************/
pub fn vectorization_rules(vector_width: usize) -> Vec<Rewrite<VecLang, ConstantFold>> {
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![];

    let mut searcher_add = Vec::new();
    let mut searcher_mul = Vec::new();
    let mut searcher_sub = Vec::new();
    let mut searcher_neg = Vec::new();

    let mut applier_1 = Vec::new();
    let mut applier_2 = Vec::new();

    for i in 0..vector_width {
        searcher_add.push(format!("( + ?a{} ?b{}) ", i, i));
        searcher_mul.push(format!("( * ?a{} ?b{}) ", i, i));
        searcher_sub.push(format!("( - ?a{} ?b{}) ", i, i));
        searcher_neg.push(format!("( - ?a{}) ", i));

        applier_1.push(format!("?a{} ", i));
        applier_2.push(format!("?b{} ", i));
    }

    let lhs_add: Pattern<VecLang> = format!("(Vec {})", searcher_add.concat()).parse().unwrap();
    let lhs_mul: Pattern<VecLang> = format!("(Vec {})", searcher_mul.concat()).parse().unwrap();
    let lhs_sub: Pattern<VecLang> = format!("(Vec {})", searcher_sub.concat()).parse().unwrap();
    let lhs_neg: Pattern<VecLang> = format!("(Vec {})", searcher_neg.concat()).parse().unwrap();

    // Parse the right-hand side patterns
    let rhs_add: Pattern<VecLang> = format!(
        "(VecAdd (Vec {}) (Vec {}))",
        applier_1.concat(),
        applier_2.concat()
    )
    .parse()
    .unwrap();
    eprintln!("{} => {}", lhs_add, rhs_add);
    let rhs_mul: Pattern<VecLang> = format!(
        "(VecMul (Vec {}) (Vec {}))",
        applier_1.concat(),
        applier_2.concat()
    )
    .parse()
    .unwrap();

    let rhs_sub: Pattern<VecLang> = format!(
        "(VecMinus (Vec {}) (Vec {}))",
        applier_1.concat(),
        applier_2.concat()
    )
    .parse()
    .unwrap();

    let rhs_neg: Pattern<VecLang> = format!("(VecNeg (Vec {}) )", applier_1.concat(),)
        .parse()
        .unwrap();

    // Push the rewrite rules into the rules vector

    rules.push(rw!(format!("add-vectorize" ); { lhs_add.clone() } => { rhs_add.clone() }));
    rules.push(rw!(format!("mul-vectorize"); { lhs_mul.clone() } => { rhs_mul.clone() }));
    rules.push(rw!(format!("sub-vectorize"); { lhs_sub.clone() } => { rhs_sub.clone() }));
    rules.push(rw!(format!("neg-vectorize"); { lhs_neg.clone() } => { rhs_neg.clone() }));
    rules
}
/*******************************************************************************/

pub fn rules0(vector_width: usize) -> Vec<Rewrite<VecLang, ConstantFold>> {
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![
        rw!("add-0-0+0-1"; "0" => "(+ 0 0)"),
        rw!("mul-0-0x0-2"; "0" => "(* 0 0)"),
        rw!("add-a-a+0-3"; "?a" => 
        "(+ ?a 0)"
        if is_leaf("?a","?a")
        ),
        rw!("mul-a-ax1"; "?a" => 
        "(* ?a 1)"
        if is_leaf("?a","?a")
        ),
        rw!("mul-deco-0";"(* ?a ?b)" => 
        "(+ 0 (* ?a ?b))"
        if is_leaf("?a","?b")
        ),
        rw!("add-0-0-0"; "0" => "(- 0 0)"),
        rw!("add-a-a-0"; "?a" => 
        "(- ?a 0)"
        if is_leaf("?a","?a")
        ),
    ];
    // Vector rules
    rules.extend(vectorization_rules(vector_width));
    rules
}

/**************************** Rules for rotations *************************/
pub fn addition_rules(vector_width: usize, expression_depth: usize) -> Vec<Rewrite<VecLang, ConstantFold>>{
    let base: usize = 2;
    eprintln!("=====>expression_depth : {}",expression_depth);
    let mut max_vector_size : usize = base.pow(expression_depth as u32 - 1) * vector_width; 
    max_vector_size = min(max_vector_size,4096);
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![
        rw!("add-0-0+0"; "0" => "(+ 0 0)"),
        rw!("add-a-a+0"; "?a" => 
        "(+ ?a 0)"
        if is_leaf("?a","?a")
        ),
        rw!("add-a*b-0+a*b"; "(* ?a ?b)" => 
        "(+ 0 (* ?a ?b))"
        ),
        rw!("add-a-b-0+a-b"; "(- ?a ?b)" => 
        "(+ 0 (- ?a ?b))"
        ),
        rw!("add--a-0+-a"; "(- ?a)" => 
        "(+ 0 (- ?a))" 
        ),
    ];
    let mut initial_vector_size : usize = 1 ;
    eprintln!("=====>max_vector_size : {}",max_vector_size);
    while initial_vector_size <= max_vector_size{
        let mut searcher_add = Vec::new();
        let mut applier_1 = Vec::new();
        let mut applier_2 = Vec::new();
        for i in 0..initial_vector_size {
            searcher_add.push(format!("( + ?a{} ?b{}) ", i, i));
            applier_1.push(format!("?a{} ", i));
            applier_2.push(format!("?b{} ", i));
        }
        let lhs_add: Pattern<VecLang> = format!("(Vec {})", searcher_add.concat()).parse().unwrap();
        // Parse the right-hand side patterns
        let rhs_add: Pattern<VecLang> = format!(
            "(VecAdd (Vec {}) (Vec {}))",
            applier_1.concat(),
            applier_2.concat()
        )
        .parse()
        .unwrap();
        // Push the rewrite rules into the rules vector
        rules.push(rw!(format!("add-vectorize-{}",initial_vector_size); { lhs_add.clone() } => { rhs_add.clone() } 
        if cond_check_not_all_values_eq0(initial_vector_size)
        ));
        initial_vector_size=initial_vector_size*2
    }
    /***************************************************************/
    max_vector_size = base.pow(expression_depth as u32 - 2) * vector_width ;
    max_vector_size = min(max_vector_size,4096);
    initial_vector_size = 1 ;
    while initial_vector_size <= max_vector_size {
        let mut searcher_add_red = Vec::with_capacity(initial_vector_size);
        let mut applier_1 = Vec::with_capacity(initial_vector_size*2);
        applier_1.resize(initial_vector_size*2,String::from(""));
        for i in 0..initial_vector_size {
            searcher_add_red.push(format!("( + ?a{} ?b{}) ", i, i));
            applier_1[i]=format!("?a{} ", i);
            applier_1[i+initial_vector_size]=format!("?b{} ", i);
        }
        let lhs_add: Pattern<VecLang> = format!("(Vec {})", searcher_add_red.concat()).parse().unwrap();
        // Parse the right-hand side patterns
        let rhs_add: Pattern<VecLang> = format!(
            "(VecAddRotS (Vec {}) {})",
            applier_1.concat(),
            initial_vector_size
        )
        .parse()
        .unwrap();
        rules.push(rw!(format!("rot-add-vectorize-{}",initial_vector_size); { lhs_add.clone() } => { rhs_add.clone() } 
        if cond_check_all_elems_composed(initial_vector_size)
        ));
        initial_vector_size=initial_vector_size*2;
    }
    /*************************************************/
    rules
}
/*************************************************************/
pub fn minus_rules(vector_width: usize, expression_depth: usize) -> Vec<Rewrite<VecLang, ConstantFold>>{
    let base: usize = 2;
    let mut max_vector_size : usize = base.pow(expression_depth as u32 - 1) * vector_width; 
    max_vector_size = min(max_vector_size,4096);
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![
        rw!("sub-0-0-0"; "0" => "(- 0 0)"),
        rw!("sub-a-a-0"; "?a" => 
        "(- ?a 0)"
        if is_leaf("?a","?a")
        ),
        rw!("sub-a*b-0-a*b"; "(* ?a ?b)" => 
        "(- 0 (* ?a ?b))"
        ),
        rw!("sub-a+b-0-a+b"; "(+ ?a ?b)" => 
        "(- 0 (+ ?a ?b))"
        ),
        rw!("sub--a-0--a"; "(- ?a)" => 
        "(- 0 (- ?a))"
        ),
    ];
    let mut initial_vector_size : usize = 1 ;
    while initial_vector_size <= max_vector_size{
        let mut searcher_sub = Vec::new();
        let mut applier_1 = Vec::new();
        let mut applier_2 = Vec::new();
        for i in 0..initial_vector_size {
            searcher_sub.push(format!("( - ?a{} ?b{}) ", i, i));
            applier_1.push(format!("?a{} ", i));
            applier_2.push(format!("?b{} ", i));
        }
        let lhs_sub: Pattern<VecLang> = format!("(Vec {})", searcher_sub.concat()).parse().unwrap();
        // Parse the right-hand side patterns
        let rhs_sub: Pattern<VecLang> = format!(
            "(VecMinus (Vec {}) (Vec {}))",
            applier_1.concat(),
            applier_2.concat()
        )
        .parse()
        .unwrap();
        // Push the rewrite rules into the rules vector
        rules.push(rw!(format!("sub-vectorize-{}",initial_vector_size); { lhs_sub.clone() } => {rhs_sub.clone()} 
        if cond_check_not_all_values_eq0(initial_vector_size)
        ));
        initial_vector_size=initial_vector_size*2
    }
    /*****************************************************/
    max_vector_size = base.pow(expression_depth as u32 - 2) * vector_width ;
    max_vector_size = min(max_vector_size,4096);
    initial_vector_size = 1 ;
    while initial_vector_size <= max_vector_size {
        let mut searcher_sub_red = Vec::with_capacity(initial_vector_size);
        let mut applier_1 = Vec::with_capacity(initial_vector_size*2);
        applier_1.resize(initial_vector_size*2,String::from(""));
        for i in 0..initial_vector_size {
            searcher_sub_red.push(format!("( - ?a{} ?b{}) ", i, i));
            applier_1[i]=format!("?a{} ", i);
            applier_1[i+initial_vector_size]=format!("?b{} ", i);
        }
        let lhs_sub: Pattern<VecLang> = format!("(Vec {})", searcher_sub_red.concat()).parse().unwrap();
        // Parse the right-hand side patterns
        let rhs_sub: Pattern<VecLang> = format!(
            "(VecMinusRotS (Vec {}) {})",
            applier_1.concat(),
            initial_vector_size
        )
        .parse()
        .unwrap();
        rules.push(rw!(format!("rot-min-vectorize-{}",initial_vector_size); { lhs_sub.clone() } => { rhs_sub.clone() } 
        if cond_check_all_elems_composed(initial_vector_size)
        ));
        initial_vector_size=initial_vector_size*2;
    }
    /***********************************************/
    rules
} 
/*************************************************************/
pub fn multiplication_rules(vector_width: usize, expression_depth: usize) -> Vec<Rewrite<VecLang, ConstantFold>>{
    let base: usize = 2;
    let mut max_vector_size : usize = base.pow(expression_depth as u32 - 1) * vector_width; 
    max_vector_size = min(max_vector_size,4096);
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![
        rw!("mul-0-0*0"; "0" => "(* 0 0)"),
        rw!("mul-a-a*1"; "?a" => 
        "(* ?a 1)"
        if is_leaf("?a","?a")
        ),
        rw!("mul-a+b-1-a+b"; "(+ ?a ?b)" => 
        "(* 1 (+ ?a ?b))"
        ),
        rw!("mul-a-b-1-a-b"; "(- ?a ?b)" => 
        "(* 1 (- ?a ?b))"
        ),
        rw!("add--a-0+-a"; "(- ?a)" => 
        "(* 1 (- ?a))"
        ),
    ];
    let mut initial_vector_size : usize = 1 ;
    while initial_vector_size <= max_vector_size{
        let mut searcher_mul = Vec::new();
        let mut applier_1 = Vec::new();
        let mut applier_2 = Vec::new();
        for i in 0..initial_vector_size {
            searcher_mul.push(format!("( * ?a{} ?b{}) ", i, i));
            applier_1.push(format!("?a{} ", i));
            applier_2.push(format!("?b{} ", i));
        }
        let lhs_mul: Pattern<VecLang> = format!("(Vec {})", searcher_mul.concat()).parse().unwrap();
        // Parse the right-hand side patterns
        let rhs_mul: Pattern<VecLang> = format!(
            "(VecMul (Vec {}) (Vec {}))",
            applier_1.concat(),
            applier_2.concat()
        )
        .parse()
        .unwrap();
        // Push the rewrite rules into the rules vector
        rules.push(rw!(format!("mul-vectorize-{}",initial_vector_size); { lhs_mul.clone() } => { rhs_mul.clone() } 
        if cond_check_not_all_values_eq1(initial_vector_size)
        ));
        initial_vector_size=initial_vector_size*2;
    }
    /**********************************************/
    max_vector_size = base.pow(expression_depth as u32 - 2) * vector_width;
    max_vector_size = min(max_vector_size,4096); 
    initial_vector_size = 1 ;
    while initial_vector_size <= max_vector_size {
        let mut searcher_mul_red = Vec::with_capacity(initial_vector_size);
        let mut applier_1 = Vec::with_capacity(initial_vector_size*2);
        applier_1.resize(initial_vector_size*2,String::from(""));
        for i in 0..initial_vector_size {
            searcher_mul_red.push(format!("( * ?a{} ?b{}) ", i, i));
            applier_1[i]=format!("?a{} ", i);
            applier_1[i+initial_vector_size]=format!("?b{} ", i);
        }
        let lhs_add: Pattern<VecLang> = format!("(Vec {})", searcher_mul_red.concat()).parse().unwrap();
        // Parse the right-hand side patterns
        let rhs_add: Pattern<VecLang> = format!(
            "(VecMulRotS (Vec {}) {})",
            applier_1.concat(),
            initial_vector_size
        )
        .parse()
        .unwrap();
        rules.push(rw!(format!("rot-mul-vectorize-{}",initial_vector_size); { lhs_add.clone() } => { rhs_add.clone() } 
        if cond_check_all_elems_composed(initial_vector_size)
        ));
        initial_vector_size=initial_vector_size*2;
    }
    /********************************************/
    rules
}
/*************************************************************/
pub fn neg_rules(vector_width : usize, expression_depth: usize) ->  Vec<Rewrite<VecLang, ConstantFold>>{
    let base: usize = 2;
    let mut max_vector_size : usize = base.pow(expression_depth as u32 - 1) * vector_width; 
    max_vector_size = min(max_vector_size,4096);

    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![
        rw!("neg-0-0+0"; "0" => "(- 0)"),
    ];
    let mut initial_vector_size : usize = 1 ;
    while initial_vector_size <= max_vector_size{
        let mut searcher_neg = Vec::new();
        let mut applier_1 = Vec::new();
        for i in 0..initial_vector_size {
            searcher_neg.push(format!("( - ?b{}) ", i));
            applier_1.push(format!("?b{} ", i));
        }
        let lhs_neg: Pattern<VecLang> = format!("(Vec {})", searcher_neg.concat()).parse().unwrap();
        let rhs_neg: Pattern<VecLang> = format!("(VecNeg (Vec {}) )", applier_1.concat(),)
        .parse()
        .unwrap();
        // Push the rewrite rules into the rules vector
        rules.push(rw!(format!("neg-vectorize-{}",initial_vector_size); { lhs_neg.clone() } => { rhs_neg.clone() }));
        initial_vector_size=initial_vector_size*2 ;
    }
    rules
}
/*************************************************************/


pub fn rot_minus_rules(vector_width: usize) -> Vec<Rewrite<VecLang, ConstantFold>>{
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![
        rw!("rot-add-0-0+0"; "0" => "(+ 0 0)"),
        rw!("rot-add-a-a+0"; "?a" => 
        "(+ ?a 0)"
        if is_leaf("?a","?a")
        ),
        rw!("rot-add-a*b-0+a*b"; "(* ?a ?b)" => 
        "(+ 0 (* ?a ?b))"
        ),
        rw!("rot-add-a-b-0+a-b"; "(- ?a ?b)" => 
        "(+ 0 (- ?a ?b))"
        ),
        rw!("rot-add--a-0+-a"; "(- ?a)" => 
        "(+ 0 (- ?a))"
        ),
    ];
    /******************************/
    let max_vector_size : usize = 1024 ; 
    let mut initial_vector_size = vector_width ;
    while initial_vector_size <= max_vector_size {
        let mut searcher_add_red = Vec::with_capacity(initial_vector_size);
        let mut applier_1 = Vec::with_capacity(initial_vector_size*2);
        applier_1.resize(initial_vector_size*2,String::from(""));
        for i in 0..initial_vector_size {
            searcher_add_red.push(format!("( + ?a{} ?b{}) ", i, i));
            applier_1[i]=format!("?a{} ", i);
            applier_1[i+initial_vector_size]=format!("?b{} ", i);
        }
        let lhs_add: Pattern<VecLang> = format!("(Vec {})", searcher_add_red.concat()).parse().unwrap();
        // Parse the right-hand side patterns
        let rhs_add: Pattern<VecLang> = format!(
            "(VecAddRotS (Vec {}) {})",
            applier_1.concat(),
            initial_vector_size
        )
        .parse()
        .unwrap();
        rules.push(rw!(format!("rot-add-vectorize-{}",initial_vector_size); { lhs_add.clone() } => { rhs_add.clone() } 
        if cond_check_all_elems_composed(initial_vector_size)
        ));
        initial_vector_size=initial_vector_size*2;
    }
    // Push the rewrite rules into the rules vector
    rules
}


/***************************************************************************************************/

pub fn vector_assoc_add_mul_rules() -> Vec<Rewrite<VecLang, ConstantFold>> {
    let rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![
        rw!("assoc-balan-add-mul-1"; 
        "(VecAdd (VecAdd (VecAdd (VecMul ?c1 ?c2) (VecMul ?d1 ?d2)) (VecMul ?b1 ?b2)) (VecMul ?a1 ?a2))" => 
        "(VecAdd (VecAdd (VecMul ?a1 ?a2) (VecMul ?b1 ?b2)) (VecAdd (VecMul ?c1 ?c2) (VecMul ?d1 ?d2)))"
        if is_vec_mul("?a1","?a2","?b1","?b2","?c1","?c2","?d1","?d2")
        ),
        rw!("assoc-balan-add-mul-2"; 
        "(VecAdd (VecMul ?a1 ?a2) (VecAdd (VecMul ?b1 ?b2) (VecAdd (VecMul ?c1 ?c2) (VecMul ?d1 ?d2))))" => 
        "(VecAdd (VecAdd (VecMul ?a1 ?a2) (VecMul ?b1 ?b2)) (VecAdd (VecMul ?c1 ?c2) (VecMul ?d1 ?d2)))"
        if is_vec_mul("?a1","?a2","?b1","?b2","?c1","?c2","?d1","?d2")
        ),
        rw!("assoc-balan-add-mul-3"; 
        "(VecAdd (VecAdd (VecMul ?a1 ?a2) (VecAdd (VecMul ?b1 ?b2) (VecMul ?c1 ?c2))) (VecMul ?d1 ?d2))" => 
        "(VecAdd (VecAdd (VecMul ?a1 ?a2) (VecMul ?b1 ?b2)) (VecAdd (VecMul ?c1 ?c2) (VecMul ?d1 ?d2)))"
        if is_vec_mul("?a1","?a2","?b1","?b2","?c1","?c2","?d1","?d2")
        ),
        rw!("assoc-balan-add-mul-4"; 
        "(VecAdd (VecAdd (VecMul ?a ?b) ?c) ?d)" => "(VecAdd (VecMul ?a ?b) (VecAdd ?c ?d))"
        if is_vec("?a","?b","?c","?c")
        ),
        rw!("assoc-balan-add-mul-5"; 
        "(VecAdd ?a (VecAdd ?b (VecMul ?c ?d)))" => "(VecAdd (VecAdd ?a ?b) (VecMul ?c ?d))"
        if is_vec("?a","?b","?c","?d")
        ),
        rw!("distribute-mul-over-add-1"; 
        "(VecMul ?a (VecAdd ?b ?c))" => "(VecAdd (VecMul ?a ?b) (VecMul ?a ?c))"
        if is_vec("?a","?b","?c","?c")
        ),
    ];
    rules
}
/***************************************************/
pub fn vector_assoc_add_min_rules() -> Vec<Rewrite<VecLang, ConstantFold>> {
    let rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![
        /*********************************************/
        rw!("assoc-balan-add-min-1"; 
        "(VecAdd (VecAdd (VecAdd (VecMinus ?c1 ?c2) (VecMinus ?d1 ?d2)) (VecMinus ?b1 ?b2)) (VecMinus ?a1 ?a2))" => 
        "(VecAdd (VecAdd (VecMinus ?a1 ?a2) (VecMinus ?b1 ?b2)) (VecAdd (VecMinus ?c1 ?c2) (VecMinus ?d1 ?d2)))"
        if is_vec_mul("?a1","?a2","?b1","?b2","?c1","?c2","?d1","?d2")
        ),
        rw!("assoc-balan-add-min-2"; 
        "(VecAdd (VecMinus ?a1 ?a2) (VecAdd (VecMinus ?b1 ?b2) (VecAdd (VecMinus ?c1 ?c2) (VecMinus ?d1 ?d2))))" => 
        "(VecAdd (VecAdd (VecMinus ?a1 ?a2) (VecMinus ?b1 ?b2)) (VecAdd (VecMinus ?c1 ?c2) (VecMinus ?d1 ?d2)))"
        if is_vec_mul("?a1","?a2","?b1","?b2","?c1","?c2","?d1","?d2")
        ),
        rw!("assoc-balan-add-min-3"; 
        "(VecAdd (VecAdd (VecMinus ?a1 ?a2) (VecAdd (VecMinus ?b1 ?b2) (VecMinus ?c1 ?c2))) (VecMinus ?d1 ?d2))" => 
        "(VecAdd (VecAdd (VecMinus ?a1 ?a2) (VecMinus ?b1 ?b2)) (VecAdd (VecMinus ?c1 ?c2) (VecMinus ?d1 ?d2)))"
        if is_vec_mul("?a1","?a2","?b1","?b2","?c1","?c2","?d1","?d2")
        ),
    ];
    rules
}
/****************************************************/
pub fn vector_assoc_min_mul_rules() -> Vec<Rewrite<VecLang, ConstantFold>> {
    let rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![
        rewrite!("assoc-balan-min-mul-1"; 
        "(VecMinus (VecMinus (VecMinus (VecMul ?c1 ?c2) (VecMul ?d1 ?d2)) (VecMul ?b1 ?b2)) (VecMul ?a1 ?a2))" => 
        "(VecMinus (VecMinus (VecMul ?a1 ?a2) (VecMul ?b1 ?b2)) (VecAdd (VecMul ?c1 ?c2) (VecMul ?d1 ?d2)))"
        if is_vec_mul("?a1","?a2","?b1","?b2","?c1","?c2","?d1","?d2")
        ),
        rewrite!("assoc-balan-min-mul-2"; 
        "(VecMinus (VecMul ?a1 ?a2) (VecMinus (VecMul ?b1 ?b2) (VecMinus (VecMul ?c1 ?c2) (VecMul ?d1 ?d2))))" => 
        "(VecMinus (VecMinus (VecMul ?a1 ?a2) (VecMul ?b1 ?b2)) (VecMinus (VecMul ?c1 ?c2) (VecMul ?d1 ?d2)))"
        if is_vec_mul("?a1","?a2","?b1","?b2","?c1","?c2","?d1","?d2")
        ),
        rewrite!("assoc-balan-min-mul-3"; 
        "(VecMinus (VecMinus (VecMul ?a1 ?a2) (VecMinus (VecMul ?b1 ?b2) (VecMul ?c1 ?c2))) (VecMul ?d1 ?d2))" => 
        "(VecMinus (VecMinus (VecMul ?a1 ?a2) (VecMul ?b1 ?b2)) (VecMinus (VecMul ?c1 ?c2) (VecMul ?d1 ?d2)))"
        if is_vec_mul("?a1","?a2","?b1","?b2","?c1","?c2","?d1","?d2")
        ),
    ];
    rules
}

/*****************************************************/

pub fn assoc_neg_rules() -> Vec<Rewrite<VecLang,ConstantFold>>{
    let rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![
        rewrite!("simplify-sub-negate"; 
        "(VecMinus ?x (VecNeg ?y))" => 
        "(VecAdd ?x ?y)"
        if is_vec("?x","?x","?y","?y")
        ),
        rewrite!("simplify-sub-negate-1"; 
        "(VecAdd ?x (VecNeg ?y))" => 
        "(VecMinus ?x ?y)"
        if is_vec("?x","?x","?y","?y")
        ),
        rewrite!("simplify-sub-negate-1-2"; 
        "(VecAdd (VecNeg ?y) ?x)" => 
        "(VecMinus ?x ?y)"
        if is_vec("?x","?x","?y","?y")
        ),
        rewrite!("simplify-add-mul-negate-1"; 
        "(VecAdd (VecMul ?x (VecNeg ?y)) ?z)" => 
        "(VecMinus ?z (VecMul ?x ?y))"
        if is_vec("?x","?x","?y","?z")
        ), 
        rewrite!("simplify-add-mul-negate-2"; 
        "(VecAdd (VecMul (VecNeg ?y) ?x) ?z)" => 
        "(VecMinus ?z (VecMul ?x ?y))"
        if is_vec("?x","?x","?y","?z")
        ),
        rewrite!("simplify-add-mul-negate-3"; 
        "(VecAdd ?z (VecMul ?x (VecNeg ?y)))" => 
        "(VecMinus ?z (VecMul ?x ?y))"
        if is_vec("?x","?x","?y","?z")
        ),
        rewrite!("simplify-add-mul-negate-4"; 
        "(VecAdd ?z (VecMul (VecNeg ?y) ?x))" => 
        "(VecMinus ?z (VecMul ?y ?x))"
        if is_vec("?x","?x","?y","?z")
        ),
        rewrite!("simplify-sub-mul-negate-1"; 
        "(VecMinus ?z (VecMul ?x (VecNeg ?y)))" => 
        "(VecAdd ?z (VecMul ?x ?y))"
        if is_vec("?x","?x","?y","?z")
        ),
        rewrite!("simplify-sub-mul-negate-2"; 
        "(VecMinus ?z (VecMul (VecNeg ?y) ?x))" => 
        "(VecAdd ?z (VecMul ?x ?y))"
        if is_vec("?x","?x","?y","?z")
        ),
        rewrite!("simplify-add-negate-2-1"; 
        "(VecAdd ?x (VecMinus (VecNeg ?y) ?z))" => 
        "(VecMinus ?x (VecAdd ?x ?y))"
        if is_vec("?x","?x","?y","?z")
        ),
        rewrite!("simplify-add-negate-2-2"; 
        "(VecAdd (VecMinus ?z (VecNeg ?y)) ?x)" => 
        "(VecMinus ?x (VecAdd ?x ?y))"
        if is_vec("?x","?x","?y","?z")
        ),
    ];
    rules
}

/******************************************************************************************/
/********************************Conditions ***********************************************/
/******************************************************************************************/
fn is_vec(var1: &'static str,var2: &'static str,var3: &'static str,var4: &'static str) -> impl Fn(&mut EGraph<VecLang, ConstantFold>, Id, &Subst) -> bool {
    let var1_str = var1.parse().unwrap();
    let var2_str = var2.parse().unwrap();
    let var3_str = var3.parse().unwrap();
    let var4_str = var4.parse().unwrap();
    move |egraph : &mut EGraph<VecLang, ConstantFold>, _, subst| {
        let nodes1 = &egraph[subst[var1_str]].nodes ;
        let nodes2 = &egraph[subst[var2_str]].nodes ;
        let nodes3 = &egraph[subst[var3_str]].nodes ;
        let nodes4 = &egraph[subst[var4_str]].nodes ;
        let is_vector1 = nodes1.iter().all(|n| matches!(n, VecLang::Vec(_) | VecLang::VecAdd(_) | VecLang::VecMul(_) | VecLang::VecMinus(_) | VecLang::VecNeg(_)));
        let is_vector2 = nodes2.iter().all(|n| matches!(n, VecLang::Vec(_) | VecLang::VecAdd(_) | VecLang::VecMul(_) | VecLang::VecMinus(_) | VecLang::VecNeg(_)));
        let is_vector3 = nodes3.iter().all(|n| matches!(n, VecLang::Vec(_) | VecLang::VecAdd(_) | VecLang::VecMul(_) | VecLang::VecMinus(_) | VecLang::VecNeg(_)));
        let is_vector4 = nodes4.iter().all(|n| matches!(n, VecLang::Vec(_) | VecLang::VecAdd(_) | VecLang::VecMul(_) | VecLang::VecMinus(_) | VecLang::VecNeg(_)));
        is_vector1&&is_vector2&&is_vector3&&is_vector4
    }
}
/****************************************/
fn is_vec_mul(var1: &'static str,var2: &'static str,var3: &'static str,var4: &'static str,var5: &'static str,var6: &'static str,var7: &'static str,var8: &'static str) -> impl Fn(&mut EGraph<VecLang, ConstantFold>, Id, &Subst) -> bool {
    let var1_str = var1.parse().unwrap();
    let var2_str = var2.parse().unwrap();
    let var3_str = var3.parse().unwrap();
    let var4_str = var4.parse().unwrap();
    let var5_str = var5.parse().unwrap();
    let var6_str = var6.parse().unwrap();
    let var7_str = var7.parse().unwrap();
    let var8_str = var8.parse().unwrap();
    move |egraph : &mut EGraph<VecLang, ConstantFold>, _, subst| {
        let nodes1 = &egraph[subst[var1_str]].nodes ;
        let nodes2 = &egraph[subst[var2_str]].nodes ;
        let nodes3 = &egraph[subst[var3_str]].nodes ;
        let nodes4 = &egraph[subst[var4_str]].nodes ;
        let nodes5 = &egraph[subst[var5_str]].nodes ;
        let nodes6 = &egraph[subst[var6_str]].nodes ;
        let nodes7 = &egraph[subst[var7_str]].nodes ;
        let nodes8 = &egraph[subst[var8_str]].nodes ;
        let is_vector1 = nodes1.iter().all(|n| matches!(n, VecLang::Vec(_) | VecLang::VecAdd(_) | VecLang::VecMul(_) | VecLang::VecMinus(_) | VecLang::VecNeg(_)));
        let is_vector2 = nodes2.iter().all(|n| matches!(n, VecLang::Vec(_) | VecLang::VecAdd(_) | VecLang::VecMul(_) | VecLang::VecMinus(_) | VecLang::VecNeg(_)));
        let is_vector3 = nodes3.iter().all(|n| matches!(n, VecLang::Vec(_) | VecLang::VecAdd(_) | VecLang::VecMul(_) | VecLang::VecMinus(_) | VecLang::VecNeg(_)));
        let is_vector4 = nodes4.iter().all(|n| matches!(n, VecLang::Vec(_) | VecLang::VecAdd(_) | VecLang::VecMul(_) | VecLang::VecMinus(_) | VecLang::VecNeg(_)));
        let is_vector5 = nodes5.iter().all(|n| matches!(n, VecLang::Vec(_) | VecLang::VecAdd(_) | VecLang::VecMul(_) | VecLang::VecMinus(_) | VecLang::VecNeg(_)));
        let is_vector6 = nodes6.iter().all(|n| matches!(n, VecLang::Vec(_) | VecLang::VecAdd(_) | VecLang::VecMul(_) | VecLang::VecMinus(_) | VecLang::VecNeg(_)));
        let is_vector7 = nodes7.iter().all(|n| matches!(n, VecLang::Vec(_) | VecLang::VecAdd(_) | VecLang::VecMul(_) | VecLang::VecMinus(_) | VecLang::VecNeg(_)));
        let is_vector8 = nodes8.iter().all(|n| matches!(n, VecLang::Vec(_) | VecLang::VecAdd(_) | VecLang::VecMul(_) | VecLang::VecMinus(_) | VecLang::VecNeg(_)));

        is_vector1&&is_vector2&&is_vector3&&is_vector4&&is_vector5&&is_vector6&&is_vector7&&is_vector8
    }
}
/***************************************/

/***************************************/
fn is_leaf(var1: &'static str,var2: &'static str) -> impl Fn(&mut EGraph<VecLang, ConstantFold>, Id, &Subst) -> bool {
    let var1_str = var1.parse().unwrap();
    let var2_str = var2.parse().unwrap();
    move |egraph : &mut EGraph<VecLang, ConstantFold>, _, subst| {
        let nodes1 = &egraph[subst[var1_str]].nodes ;
        let nodes2 = &egraph[subst[var2_str]].nodes ;
        let is_leaf1 = nodes1.iter().all(|enode| enode.children().is_empty());    
        let is_leaf2 = nodes2.iter().all(|enode| enode.children().is_empty());
        let inf = (nodes1.len()==1) && (nodes2.len() == 1) ; 
        //let inf = (nodes1.len()==1) && (nodes2.len() == 1) && !has_vec_parent(egraph, subst[var1_str]) && !has_vec_parent(egraph, subst[var2_str]); 
        // Return true if both e-classes are leaves
        is_leaf1&&is_leaf2&&inf
    }
}
/***************************************/
pub fn cond_check_not_all_values_eq1(vector_width: usize)-> impl Fn(&mut EGraph<VecLang, ConstantFold>, Id, &Subst) -> bool {
    move |egraph : &mut EGraph<VecLang, ConstantFold>, _, subst| {
        (0..vector_width).any(|i| {
            //let var2_str = var2.parse().unwrap();
            let bi = subst[format!("?b{}", i).as_str().parse().unwrap()];
            let ai = subst[format!("?a{}", i).as_str().parse().unwrap()];
            (!&egraph[bi].nodes.iter().any(|node| matches!(node, VecLang::Num(1))))&&(!&egraph[ai].nodes.iter().any(|node| matches!(node, VecLang::Num(1))))
        })
    }
}
/***************************************/
pub fn cond_check_not_all_values_eq0(vector_width: usize)-> impl Fn(&mut EGraph<VecLang, ConstantFold>, Id, &Subst) -> bool {


    move |egraph : &mut EGraph<VecLang, ConstantFold>, _, subst| {
        (0..vector_width).any(|i| {
            //let var2_str = var2.parse().unwrap();
            let bi = subst[format!("?b{}", i).as_str().parse().unwrap()];
            let _ai = subst[format!("?a{}", i).as_str().parse().unwrap()];
            (!&egraph[bi].nodes.iter().any(|node| matches!(node, VecLang::Num(0))))&&(!&egraph[bi].nodes.iter().any(|node| matches!(node, VecLang::Num(0))))
        })
    }
}
/***************************************/
pub fn cond_check_all_elems_composed(vector_width: usize)-> impl Fn(&mut EGraph<VecLang, ConstantFold>, Id, &Subst) -> bool {
    move |egraph : &mut EGraph<VecLang, ConstantFold>, _, subst| {
        (0..vector_width).all(|i| {
            //let var2_str = var2.parse().unwrap();
            let bi = subst[format!("?b{}", i).as_str().parse().unwrap()];
            let ai = subst[format!("?a{}", i).as_str().parse().unwrap()];
            (egraph[bi].nodes.iter().all(|node| matches!(node, VecLang::Add(_)| VecLang::Mul(_) | VecLang::Minus(_) )))&&(egraph[ai].nodes.iter().all(|node| matches!(node, VecLang::Add(_) | VecLang::Mul(_) | VecLang::Minus(_) )))
        })
    }
}