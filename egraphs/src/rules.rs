use std::usize;

use crate::{
    extractor::Extractor,
    veclang::{ConstantFold, Egraph, VecLang},
    runner::Runner,
    cost::VecCostFn,
};
use std::collections::HashMap; 
use std::collections::HashSet;
use egg::rewrite as rw;
use egg::*;
// Check if all the variables, in this case memories, are equivalent

/// Run the rewrite rules over the input program and return the best (cost, program)
use std::time::Instant;

pub fn run(
    prog: &RecExpr<VecLang>,
    timeout: u64,
    vector_width: usize,
) -> (usize, RecExpr<VecLang>) {
    // Initialize the rule set based on the vector width
    let rules = rules(vector_width);
    let mut iteration_count = 0;
    //let mut stop_flag = false;

    // Start timing the e-graph building process
    let start_time = Instant::now();

    // Initialize the e-graph with constant folding enabled and add a zero literal
    let mut init_eg = Egraph::new(ConstantFold);
    init_eg.add(VecLang::Num(0));

    type MyRunner = Runner<VecLang, ConstantFold>;

    let runner = MyRunner::new(Default::default())
        .with_egraph(init_eg)
        .with_expr(&prog)
        .with_node_limit(10_000)
        .with_time_limit(std::time::Duration::from_secs(timeout))
        .with_iter_limit(10_000)
        .with_hook({
            move |runner| {
                /* print the egraph after each iteration */
                iteration_count += 1;
                // eprintln!("egraph in iteration : {:} ***********", iteration_count);
                // print_egraph(runner.egraph.clone());
                Ok(())
            }
        }).run(&rules);

        /* for the rules , if the rule is expensive we add the prefix exp to its name */

    // Stop timing after the e-graph is built
    let build_time = start_time.elapsed();
    //eprintln!("E-graph built in {:?}", build_time);

    // Print the reason for stopping to STDERR
    eprintln!(
        "Stopped after {} iterations, reason: {:?}",
        runner.iterations.len(),
        runner.stop_reason
    );

    // Extract the e-graph and the root node
    let (eg, root) = (runner.egraph, runner.roots[0]);
    //eprintln!("final number of enodes : {:?}", eg.total_size());

    let mut best_cost = usize::MAX;
    let mut best_expr: RecExpr<VecLang> = RecExpr::default();
    //eprintln!("begining of extraction 0 .... ");

    /* we have 3 ways fot the extraction:
        1) greedy_extraction: takes decisions locally
        2) exhaustive_extraction: exploring all possibilities
        3) sa_extraction: based on simulating annealing metaheuristic
    */

    /************************************ greedy extraction ******************************************/
    let start_extract_time = Instant::now();
    let mut extractor = Extractor::new(&eg, VecCostFn { egraph: &eg }, root);
    (best_cost, best_expr) = extractor.find_best(root);
    let extract_time = start_extract_time.elapsed();

    /********************************** Exhaustive extraction ***************************************/
    // let start_extract_time = Instant::now();
    // let mut extractor = Extractor::new(&eg);
    // extractor.find_best(
    //     vec![root],
    //     HashMap::new(),
    //     root,
    //     0,
    //     0,
    //     vec![],
    //     &mut best_cost,
    //     &mut best_expr,
    //     &mut HashMap::new(), 
    // );
    // let extract_time = start_extract_time.elapsed();

    /************************************ SA extraction *************************************************/
    // let start_extract_time = Instant::now();
    // let mut extractor = Extractor::new(&eg);
    // let mut n_cost:usize = 0;

    // // Parameters for simulated annealing
    // let max_iteration = 500000;
    // let initial_temp = 100000.0;
    // let cooling_rate = 0.95;

    // (best_cost, best_expr) = extractor.find_best(
    //     &eg,
    //     root,
    //     max_iteration,
    //     initial_temp,
    //     cooling_rate,
    // );

    // let extract_time = start_extract_time.elapsed();

    // Stop timing after the extraction is complete
    //eprintln!("display final results");
    //eprintln!("Expression extraction took {:?}", extract_time);
    //eprintln!("Final cost is {}", best_cost);
    //eprintln!("Extracted Expression : {}", best_expr);

    // Return the extracted cost and expression
    (best_cost, best_expr)

}

pub fn print_egraph(
    egraph: Egraph
)
{

    eprintln!("***************egraph******************");

                for eclass in egraph.classes() {
                    // Print the e-class ID
                    eprint!("E-Class {{Id: {}}} =", eclass.id);
            
                    // Iterate over all enodes in the e-class and print them
                    for enode in &eclass.nodes {
                        eprint!(" {:?}", enode);
                    }
            
                    // Newline after each e-class
                    eprintln!();
                }
    // Create a map to hold the connections for each eclass
    let mut connections: HashMap<Id, HashSet<Id>> = HashMap::new();

    for class in egraph.classes() {
        let class_id = class.id;    

        // Initialize the set of connections if not already present
        let mut class_connections = HashSet::new();
        // eprintln!("The size of this eclass with id {:?} is : {:?}", class_id, class.len());

        for (_node_index, node) in class.iter().enumerate() {
            // Print the content of each enode
            // eprintln!("  Enode {}: {:?}", node_index + 1, node);
            for child in node.children() {
                // eprintln!("    Child: {:?}", child);
                // Add child to the list of connections
                class_connections.insert(*child);
            }
        }

        connections.insert(class_id, class_connections);
    }

    // Print the graph in the terminal
    for (class_id, class_connections) in &connections {
        let connections_str: String = class_connections
            .iter()
            .map(|id| id.to_string())
            .collect::<Vec<String>>()
            .join(", ");

        eprintln!("Class {} linked to {}", class_id, connections_str);
    }
}


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
    /*
        (Vec ( + ?a0 ?b0 ) ( + ?a1 ?b1 ) ( + ?a2 ?b2 ) ( + ?a3 ?b3 )) =>
        (VecAdd (Vec ?a0 ?a1 ?a2 ?a3) (Vec ?b0 ?b1 ?b2 ?b3))
        
        (Vec ( - ?a0 ?b0 ) ( - ?a1 ?b1 ) ( - ?a2 ?b2 ) ( - ?a3 ?b3 )) =>
        (VecMinus (Vec ?a0 ?a1 ?a2 ?a3) (Vec ?b0 ?b1 ?b2 ?b3))

        (Vec ( - ?a0 ) ( - ?a1 ) ( - ?a2 ) ( - ?a3 )) =>
        (VecNeg (Vec ?a0 ?a1 ?a2 ?a3))

    */
    rules.push(rw!(format!("add-vectorize" ); { lhs_add.clone() } => { rhs_add.clone() }));
    rules.push(rw!(format!("mul-vectorize"); { lhs_mul.clone() } => { rhs_mul.clone() }));
    rules.push(rw!(format!("sub-vectorize"); { lhs_sub.clone() } => { rhs_sub.clone() }));
    rules.push(rw!(format!("neg-vectorize"); { lhs_neg.clone() } => { rhs_neg.clone() }));
    rules
}

pub fn rotation_rules(vector_width: usize, rotation_amount: usize) -> Vec<Rewrite<VecLang, ConstantFold>> {
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![];

    /****************** zakaria implementation *******************************/
    // let searcher_combine: Pattern<VecLang> = "( << ( << ?a ?b ) ?c)".parse().unwrap();
    // let applier_combine: Pattern<VecLang> = " (<< ?a ( + ?b ?c) )".parse().unwrap();
    // let rule_combine: Rewrite<VecLang, ConstantFold> =
    //     rw!("rotation_combine" ; {searcher_combine} => {applier_combine});
    // rules.push(rule_combine);
    let vector_width: usize = vector_width; // Store vector width in a constant

    let lhs = format!(
        "(Vec {})",
        (0..vector_width)
            .map(|i| format!("?a{} ", i))
            .collect::<String>()
    );

    let searcher: Pattern<VecLang> = lhs.parse().unwrap();

    for i in 1..vector_width {
        let rhs = format!(
            "(<< (Vec {}) {})",
            (0..vector_width)
                .map(|j| format!("?a{} ", (i + j) % vector_width))
                .collect::<String>(),
            vector_width - i
        );
        let applier: Pattern<VecLang> = rhs.parse().unwrap();
        let rule: Vec<Rewrite<VecLang, ConstantFold>> =
            rw!(format!("exp-rotations-{}", i); { searcher.clone() } <=> { applier.clone() });
        rules.extend(rule);
    }

    rules
    

    /*****************************  my implementation ********************************************/
    // let vector_width: usize = vector_width;

    // let actual_rotation_amount = if rotation_amount >= vector_width {
    //     vector_width - 1
    // } else {
    //     rotation_amount
    // };

    // let lhs = format!(
    //     "(Vec {})",
    //     (0..vector_width)
    //         .map(|i| format!("?a{} ", i))
    //         .collect::<String>()
    // );

    // let searcher: Pattern<VecLang> = lhs.parse().unwrap();
    // let rhs = format!(
    //     "(<< (Vec {}) {})",
    //     (0..vector_width)
    //         .map(|j| format!("?a{} ", (actual_rotation_amount + j) % vector_width))
    //         .collect::<String>(),
    //         actual_rotation_amount
    // );
    // let applier: Pattern<VecLang> = rhs.parse().unwrap();
    // let rule: Vec<Rewrite<VecLang, ConstantFold>> = rw!(format!("rotations-{}", actual_rotation_amount); { searcher.clone() } <=> { applier.clone() });
    // rules.extend(rule);
    // rules
}

pub fn split_vectors(vector_width: usize) -> Vec<Rewrite<VecLang, ConstantFold>> {
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![];

    // Store vector width in a constant

    /************************** Zakaria implementaion *********************************/

    let lhs = format!(
        "(Vec {})",
        (0..vector_width)
            .map(|i| format!("?a{} ", i))
            .collect::<String>()
    );

    let searcher: Pattern<VecLang> = lhs.parse().unwrap();

    for i in 0..vector_width {
        let vector1_add = format!(
            "(Vec {})",
            (0..vector_width)
                .map(|j| if i == j {
                    "0 ".to_string()
                } else {
                    format!("?a{} ", j)
                })
                .collect::<String>()
        );
        let vector1_mul = format!(
            "(Vec {})",
            (0..vector_width)
                .map(|j| if i == j {
                    "1 ".to_string()
                } else {
                    format!("?a{} ", j)
                })
                .collect::<String>()
        );

        let vector2_add = format!(
            "(Vec {})",
            (0..vector_width)
                .map(|j| if i == j {
                    format!("?a{} ", j)
                } else {
                    "0 ".to_string()
                })
                .collect::<String>()
        );
        let vector2_mul = format!(
            "(Vec {})",
            (0..vector_width)
                .map(|j| if i == j {
                    format!("?a{} ", j)
                } else {
                    "1 ".to_string()
                })
                .collect::<String>()
        );

        let rhs_add = format!("(VecAdd {} {})", vector1_add, vector2_add);
        let rhs_mul = format!("(VecMul {} {})", vector1_mul, vector2_mul);
        let applier_add: Pattern<VecLang> = rhs_add.parse().unwrap();
        let applier_mul: Pattern<VecLang> = rhs_mul.parse().unwrap();

        rules.push(rw!(format!("exp-split-add-{}", i); {  searcher.clone()} => {  applier_add}));
        rules.push(rw!(format!("exp-split-mul-{}", i); {  searcher.clone()} => {  applier_mul}))
    }

    rules

    /************************* my implementation *********************************/

    // let lhs = format!(
    //     "(Vec {})",
    //     (0..vector_width)
    //         .map(|i| format!("?a{} ", i))
    //         .collect::<String>()
    // );

    // let searcher: Pattern<VecLang> = lhs.parse().unwrap();

    // let vector1_add = format!(
    //     "(Vec {})",
    //     (0..vector_width)
    //         .map(|i| if i % 2 == 0 {
    //             format!("?a{} ", i)
    //         } else {
    //             "0 ".to_string()
    //         })
    //         .collect::<String>()
    // );
    
    // let vector2_add = format!(
    //     "(Vec {})",
    //     (0..vector_width)
    //         .map(|i| if i % 2 != 0 {
    //             format!("?a{} ", i)
    //         } else {
    //             "0 ".to_string()
    //         })
    //         .collect::<String>()
    // );

    // let vector1_mul = format!(
    //     "(Vec {})",
    //     (0..vector_width)
    //         .map(|i| if i % 2 == 0 {
    //             format!("1 ")
    //         } else {
    //             format!("?a{} ", i)
    //         })
    //         .collect::<String>()
    // );
    
    // let vector2_mul = format!(
    //     "(Vec {})",
    //     (0..vector_width)
    //         .map(|i| if i % 2 != 0 {
    //             format!("1 ")
    //         } else {
    //             format!("?a{} ", i)
    //         })
    //         .collect::<String>()
    // );

    // let rhs_add = format!("(VecAdd {} {})", vector1_add, vector2_add);
    // let rhs_mul = format!("(VecMul {} {})", vector1_mul, vector2_mul);

    // let applier_add: Pattern<VecLang> = rhs_add.parse().unwrap();
    // let applier_mul: Pattern<VecLang> = rhs_mul.parse().unwrap();

    // rules.push(rw!("split-add"; { searcher.clone() } => { applier_add }));
    // rules.push(rw!("split-mul"; { searcher.clone() } => { applier_mul }));

    // rules
}

pub fn commutativity_rules(vector_width: usize) -> Vec<Rewrite<VecLang, ConstantFold>> {
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![];

    for i in (0..vector_width).step_by(2) {
        // Create the lhs and rhs expressions directly as strings
        let lhs = format!("(+ (* a{} b{}) c{})", i, i, i);
        let rhs = format!("(+ c{} (* a{} b{}))", i, i, i);

        // Parse the expressions into patterns
        let lhs_pattern: Pattern<VecLang> = lhs.parse().unwrap();
        let rhs_pattern: Pattern<VecLang> = rhs.parse().unwrap();

        // Add the rewrite rule using a literal string for the rule name
        rules.push(rw!(format!("exp-assoc-{}", i); lhs_pattern => rhs_pattern));
    }

    rules
}


pub fn operations_rules(vector_width: usize) -> Vec<Rewrite<VecLang, ConstantFold>> {
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![];

    // Store vector width in a constant

    // Iterate over each possible position in the vector
    for i in 0..vector_width {
        // Initialize vectors to store different patterns
        let mut vector_add = Vec::new();
        let mut vector_mul = Vec::new();
        let mut vector_sub = Vec::new();
        let mut vector_neg = Vec::new();
        let mut vector1 = Vec::new();
        let mut vector2 = Vec::new();
        let mut vector1_neg = Vec::new();
        let mut vector2_neg = Vec::new();
        let mut vector2_mul = Vec::new();

        // Iterate over each element in the vector
        for j in 0..vector_width {
            if i == j {
                // When i equals j, insert the operations
                vector_add.push(format!("( + ?a{}1 ?a{}2) ", j, j));
                vector_mul.push(format!("( * ?a{}1 ?a{}2) ", j, j));
                vector_sub.push(format!("( - ?a{}1 ?a{}2) ", j, j));
                vector_neg.push(format!("( - ?a{}) ", j));
                vector1_neg.push("0 ".to_string());
                vector2_neg.push(format!("?a{}  ", j));
                vector1.push(format!("?a{}1 ", j));
                vector2.push(format!("?a{}2 ", j));
                vector2_mul.push(format!("?a{}2 ", j));
            } else {
                // When i does not equal j, insert the vector elements
                vector_add.push(format!("?a{} ", j));
                vector_mul.push(format!("?a{} ", j));
                vector_sub.push(format!("?a{} ", j));
                vector_neg.push(format!("?a{} ", j));
                vector1.push(format!("?a{} ", j));
                vector1_neg.push(format!("?a{} ", j));
                vector2_neg.push("0 ".to_string());
                vector2_mul.push("1 ".to_string());
                vector2.push("0 ".to_string());
            }
        }

        // Parse the left-hand side patterns
        let lhs_add: Pattern<VecLang> = format!("(Vec {})", vector_add.concat()).parse().unwrap();
        let lhs_mul: Pattern<VecLang> = format!("(Vec {})", vector_mul.concat()).parse().unwrap();
        let lhs_sub: Pattern<VecLang> = format!("(Vec {})", vector_sub.concat()).parse().unwrap();
        let lhs_neg: Pattern<VecLang> = format!("(Vec {})", vector_neg.concat()).parse().unwrap();

        // Parse the right-hand side patterns
        let rhs_add: Pattern<VecLang> = format!(
            "(VecAdd (Vec {}) (Vec {}))",
            vector1.concat(),
            vector2.concat()
        )
        .parse()
        .unwrap();

        let rhs_mul: Pattern<VecLang> = format!(
            "(VecMul (Vec {}) (Vec {}))",
            vector1.concat(),
            vector2_mul.concat()
        )
        .parse()
        .unwrap();

        let rhs_sub: Pattern<VecLang> = format!(
            "(VecMinus (Vec {}) (Vec {}))",
            vector1.concat(),
            vector2.concat()
        )
        .parse()
        .unwrap();

        let rhs_neg: Pattern<VecLang> = format!(
            "(VecMinus (Vec {}) (Vec {}))",
            vector1_neg.concat(),
            vector2_neg.concat()
        )
        .parse()
        .unwrap();

        // Push the rewrite rules into the rules vector
        rules.push(rw!(format!("add-split-{}", i); { lhs_add.clone() } => { rhs_add.clone() }));
        rules.push(rw!(format!("mul-split-{}", i); { lhs_mul.clone() } => { rhs_mul.clone() }));
        rules.push(rw!(format!("sub-split-{}", i); { lhs_sub.clone() } => { rhs_sub.clone() }));
        rules.push(rw!(format!("neg-split-{}", i); { lhs_neg } => { rhs_neg }));
    }

    rules
}
/****************************************************************************************/
/****************************************************************************************/
pub fn rules(vector_width: usize) -> Vec<Rewrite<VecLang, ConstantFold>> {
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![
        rw!("add-0"; "(+ 0 ?a)" => "?a"),
        rw!("add-0-2"; "(+ ?a 0)" => "?a"),
        rw!("mul-0"; "(* 0 ?a)" => "0"),
        rw!("mul-0-2"; "(* ?a 0)" => "0"),
        rw!("mul-1"; "(* 1 ?a)" => "?a"),
        rw!("mul-1-2"; "(* ?a 1)" => "?a"),
        rw!("comm-factor-1"; "(+ (* ?a0 ?b0) (* ?a0 ?c0))" => "(* ?a0 (+ ?b0 ?c0))"),
        rw!("comm-factor-2"; "(+ (* ?b0 ?a0) (* ?c0 ?a0))" => "(* ?a0 (+ ?b0 ?c0))"),
    ];

    // Vector rules
    rules.extend(vectorization_rules(vector_width));
    let rotation_rules = rotation_rules(vector_width, 2);
    let operations_rules = operations_rules(vector_width);
    let split_vectors = split_vectors(vector_width);
    let assoc = commutativity_rules(vector_width);
    //rules.extend(rotation_rules);
    //rules.extend(operations_rules);
    //rules.extend(split_vectors);
    //rules.extend(assoc);

    rules.extend(vec![
        //  Basic associativity/commutativity/identities 8102 / expensive rules
        // rw!("commute-Add"; "(+ ?a ?b)" => "(+ ?b ?a)"),
        // rw!("commute-Mul"; "(* ?a ?b)" => "(* ?b ?a)"),
        // rw!("assoc-Add"; "(+ (+ ?a ?b) ?c)" => "(+ ?a ( + ?b ?c))"),
        // rw!("assoc-Mul"; "(* ( * ?a ?b) ?c)" => "(* ?a ( * ?b ?c))"),
        // rw!("commute-vecadd"; "(VecAdd ?a ?b)" => "(VecAdd ?b ?a)"),
        // rw!("commute-vecmul"; "(VecMul ?a ?b)" => "(VecMul ?b ?a)"),
        // rw!("assoc-vecadd"; "(VecAdd (VecAdd ?a ?b) ?c)" => "(VecAdd ?a (VecAdd ?b ?c))"),
        // rw!("assoc-vecmul"; "(VecMul (VecMul ?a ?b) ?c)" => "(VecMul ?a (VecMul ?b ?c))"),
        rw!("exp-comm-mul-add"; "(+ ?c0 (* ?a0 ?b0))" => "(+ (* ?a0 ?b0 ) ?c0)"),   // this is an optimization of the commutativiy rule
        rw!("associativity"; "(* ?a0 (* ?b0 ?c0))" => "(* (* ?b0 ?c0) ?a0)"),
        rw!("commutativity"; "(+ ?a0 (+ ?b0 ?c0))" => "(+ (+ ?b0 ?c0) ?c0)"),
    ]);
    rules
}