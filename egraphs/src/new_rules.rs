use std::usize;
use std::collections::HashSet;
use std::collections::HashMap;

use crate::{
    extractor::Extractor,
    veclang::{ConstantFold, VecLang},
    runner::Runner,
};
use egg::rewrite as rw;
use egg::*;
//use crate::veclang::VecLang::Mul;

// Check if all the variables, in this case memories, are equivalent

/// Run the rewrite rules over the input program and return the best (cost, program)
pub fn run(
    prog: &RecExpr<VecLang>,
    timeout: u64,
    vector_width: usize,
) -> (usize, RecExpr<VecLang>) {
    // Initialize the rule set based on the vector width
    let rules = rules(vector_width);

    // let start = "(Vec 
    // (+ (+ (* a0 (* b0 c0)) d0) (+ (* a1 (* b1 c1)) (* a3 (* b3 c3))))
    // (+ (* a2 (* b2 c2)) d1)
    // (+ d2 (* a4 (* b4 c4)))
    // )".parse().unwrap();

    // let start = "
    //     (Vec 
    //     (+ (+ (* a0 b0) c0) (* d0 e0))
    //     (+ (+ a1 b1) (+ (* c1 d1) (* e1 f1)))
    //     (+ (+ (* a2 b2) c2) (* e2 f2))
    // )
    // ".parse().unwrap();

    // Initialize the e-graph with constant folding enabled and add a zero literal
    let mut init_eg = EGraph::new(ConstantFold {
        blacklist_nodes: HashSet::new(),
        precedent_map: HashMap::new(),
    });

    let mut switch_eg = EGraph::new(ConstantFold {
        blacklist_nodes: HashSet::new(),
        precedent_map: HashMap::new(),
    });

    let mut iteration_count = 0;

    pub struct MyIterData {
        enode_eclasses_map: HashMap<(VecLang, Id), HashSet<Id>>,
    }

    type MyRunner = Runner<VecLang, ConstantFold, MyIterData>;
    
    
    impl IterationData<VecLang, ConstantFold> for MyIterData {
    fn make(runner: &MyRunner) -> Self {
        let mut enode_eclasses_map = HashMap::new();

        // Iterate over each e-class in the e-graph
        for eclass in runner.egraph.classes() {
            // Iterate over each e-node in the e-class
            for enode in eclass.iter() {
                // Canonicalize the e-node's children
                let canonical_enode = enode.clone().map_children(|id| runner.egraph.find(id));

                // Create a set to hold the e-classes that the e-node's children belong to
                let mut eclasses_set = HashSet::new();

                // For each child of the canonical e-node, find its e-class
                for child in canonical_enode.children() {
                    let eclass_id = runner.egraph.find(*child);
                    eclasses_set.insert(eclass_id);
                }

                // Get the canonical e-class ID for the current enode
                let eclass_id = runner.egraph.find(eclass.id);

                // Add the canonical e-class itself to the eclasses_set
                eclasses_set.insert(eclass_id);

                // Insert the canonicalized enode and its corresponding e-classes into the map
                enode_eclasses_map.insert((canonical_enode, eclass_id), eclasses_set);
            }
        }

            MyIterData {
                enode_eclasses_map,
            }
        }
    }
    
    

    init_eg.add(VecLang::Num(0));
    switch_eg.add(VecLang::Num(0));

    fn get_new_nodes(
        previous_nodes: &HashMap<(VecLang, Id), HashSet<Id>>,
        current_nodes: &HashMap<(VecLang, Id), HashSet<Id>>,
    ) -> HashMap<(VecLang, Id), HashSet<Id>> {
        let mut new_nodes = HashMap::new();

        for (enode, eclasses) in current_nodes {
            if !previous_nodes.contains_key(enode) {
                new_nodes.insert(enode.clone(), eclasses.clone());
            }
        }
        new_nodes
    }

    // Configure the runner with given limits
    let runner = MyRunner::new(Default::default())
    .with_egraph(init_eg)
    .with_expr(&prog)
    .with_node_limit(10_000_000)
    .with_time_limit(std::time::Duration::from_secs(timeout))
    .with_iter_limit(10_000)
    .with_hook(move |runner| {
        // iteration_count += 1;

        // if iteration_count >= 2 {

                //     // Detect cycles
                //     let root = runner.egraph.find(runner.roots[0]); // Assuming you have a root
                //     let mut visited: HashSet<Id> = HashSet::new();
                //     let mut cycles: Vec<Vec<VecLang>> = Vec::new();
                //     Extractor::get_cycles(
                //         &runner.egraph,
                //         root,
                //         &mut visited,
                //         &mut cycles
                //     );

                //     runner.egraph.analysis.blacklist_nodes.insert(cycles);
                //     let cycle_in_blacklist = .iter()
                //     .any(|id| runner.egraph.analysis.blacklist_nodes.contains(id));

                //     // Handle cycles and remove specific nodes
                //     if !cycles.is_empty() && !cycle_in_blacklist{
                //         eprintln!("Applying removals...");
                //         runner.egraph = switch_eg.clone();
                //     } else {
                //         eprintln!("No removals...");
                //         switch_eg = runner.egraph.clone();
                // }


        //     if let Some(last_iteration_data) = runner.iterations.last() {
        //         let new_nodes = get_new_nodes(
        //             &runner.egraph.analysis.precedent_map,
        //             &last_iteration_data.data.enode_eclasses_map,
        //         );

        //         // Step 1: Collect enodes to remove
        //         let mut enodes_to_remove: HashMap<Id, Vec<VecLang>> = HashMap::new();
                
        //         for ((enode, eclass), _eclasses) in &new_nodes {
        //             enodes_to_remove.entry(*eclass).or_default().push(enode.clone());
        //         }

        //         // Print the collected enodes to be removed
        //         // eprintln!("Enodes collected:");
        //         //     for (eclass, enodes) in &enodes_to_remove {
        //         //         let enodes_str = enodes
        //         //             .iter()
        //         //             .map(|n| format!("{:?}", n))
        //         //             .collect::<Vec<String>>()
        //         //             .join(", ");
        //         //         eprintln!(
        //         //             "E-Class ID: {:?} -> Enodes : {}",
        //         //             eclass, enodes_str
        //         //         );
        //         //     }

        //         // Step 2: Apply removals after the loop
        //         // if iteration_count == 5 {
        //             // eprintln!("Enodes to be removed:");
        //             // for (eclass, enodes) in &enodes_to_remove {
        //             //     let enodes_str = enodes
        //             //         .iter()
        //             //         .map(|n| format!("{:?}", n))
        //             //         .collect::<Vec<String>>()
        //             //         .join(", ");
        //             //     eprintln!(
        //             //         "E-Class ID: {:?} -> Enodes to remove: {}",
        //             //         eclass, enodes_str
        //             //     );
        //             // }
        //             // Detect cycles
        //             let root = runner.egraph.find(runner.roots[0]); // Assuming you have a root
        //             let mut visited: HashSet<Id> = HashSet::new();
        //             let mut cycles: Vec<Vec<VecLang>> = Vec::new();
        //             Extractor::get_cycles(
        //                 &runner.egraph,
        //                 root,
        //                 &mut visited,
        //                 &mut cycles
        //             );

        //             eprintln!("All detected cycles: {:?} in iteration {:?}", cycles, iteration_count);
        //             //eprintln!("\n ---- printing egraph before removals ----");
        //             //Extractor::print_egraph(&runner.egraph);
        //             // eprintln!("Applying removals...");
        //             // for (eclass, enodes) in enodes_to_remove  {
        //             //     let class = &runner.egraph[eclass];
        //             //     eprintln!("size of this eclass {:?} is {:?}", eclass, class.len());
        //             //     //let node_to_remove = VecLang::from(Mul([14.into(), 5.into()]));
        //             //     let _ = &mut runner.egraph[eclass].nodes.retain(|n| !enodes.contains(n));
                       

                       
    
        //             //     //eprintln!("---> number of enodes in elcass {:?} after removing: {:?}", eclass, eclass.len());
        //             // }
        //             eprintln!("------ Iteration {:} ---------", iteration_count);

        //             for eclass in runner.egraph.classes() {
        //                 // Print the e-class ID
        //                 eprint!("E-Class {{Id: {}}} =", eclass.id);
                
        //                 // Iterate over all enodes in the e-class and print them
        //                 for enode in &eclass.nodes {
        //                     eprint!(" {:?}", enode);
        //                 }
                
        //                 // Newline after each e-class
        //                 eprintln!();
        //             }

        //             // Handle cycles and remove specific nodes
        //             if !cycles.is_empty() {
        //                 eprintln!("Applying removals...");
        //                 for (eclass, enodes) in enodes_to_remove  {
        //                 let class = &runner.egraph[eclass];
        //                 //eprintln!("size of this eclass {:?} is {:?}", eclass, class.len());
        //                 //let node_to_remove = VecLang::from(Mul([14.into(), 5.into()]));
        //                 let _ = &mut runner.egraph[eclass].nodes.retain(|n| !enodes.contains(n));
    
        //             // }
        //             }
        //             runner.egraph.rebuild();
        //         }

                   
        //         // }

        //         runner.egraph.analysis.precedent_map = last_iteration_data.data.enode_eclasses_map.clone();
        //     } else {
        //         eprintln!("Warning: No iteration data available.");
        //     }
        // }
        
        Ok(())
    })
    .run(&rules);

    // Print the reason for stopping to STDERR
    eprintln!(
        "Stopped after {} iterations, reason: {:?}",
        runner.iterations.len(),
        runner.stop_reason
    );

    // Detect cycles
                    let root = runner.egraph.find(runner.roots[0]); // Assuming you have a root
                    let mut visited: HashSet<Id> = HashSet::new();
                    let mut cycles: Vec<Vec<VecLang>> = Vec::new();
                    Extractor::get_cycles(
                        &runner.egraph,
                        root,
                        &mut visited,
                        &mut cycles
                    );

                    eprintln!("All detected cycles: {:?}", cycles);

    // Extract the e-graph and the root node
    let (eg, root) = (runner.egraph, runner.roots[0]);

    //Extractor::get_cycles(&eg, root);
    eprintln!("\n ---- printing egraph ----");
    Extractor::print_egraph(&eg);
    //Extractor::finding_all_descendents(&eg, root);

    // Prepare for extraction, always add the literal zero
    let mut extractor = Extractor::new(&eg);

    // Collect all e-classes
    let eclasses = eg.classes();

    let mut combinations = 1;
    for eclass in eclasses { 
        if eclass.len() > 0 {
            combinations *= eclass.nodes.len();
        }
    }
    eprintln!("number of combinations : {}", combinations);

   
    // Initialize cost and expression for extraction
    let mut best_cost = usize::MAX;
    let mut best_expr: RecExpr<VecLang> = RecExpr::default();

    // Perform extraction from all combinations of e-nodes

    extractor.find_best(
        vec![root],
        HashMap::new(),
        root,
        0,
        0,
        vec![],
        &mut best_cost,
        &mut best_expr,
    );

    // Return the extracted cost and expression
    let mut best_cost = usize::MAX;
    let mut best_expr: RecExpr<VecLang> = RecExpr::default();
    (best_cost, best_expr)
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

    rules.push(rw!(format!("add-vectorize" ); { lhs_add.clone() } => { rhs_add.clone() }));
    rules.push(rw!(format!("mul-vectorize"); { lhs_mul.clone() } => { rhs_mul.clone() }));
    rules.push(rw!(format!("sub-vectorize"); { lhs_sub.clone() } => { rhs_sub.clone() }));
    rules.push(rw!(format!("neg-vectorize"); { lhs_neg.clone() } => { rhs_neg.clone() }));
    rules
}
pub fn rotation_rules(vector_width: usize) -> Vec<Rewrite<VecLang, ConstantFold>> {
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![];
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
            rw!(format!("rotations-{}", i); { searcher.clone() } <=> { applier.clone() });
        rules.extend(rule);
    }

    rules
}

pub fn split_vectors(vector_width: usize) -> Vec<Rewrite<VecLang, ConstantFold>> {
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![];

    // Store vector width in a constant

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

        rules.push(rw!(format!("split-add-{}", i); {  searcher.clone()} => {  applier_add}));
        rules.push(rw!(format!("split-mul-{}", i); {  searcher.clone()} => {  applier_mul}))
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
        rules.extend(rw!(format!("add-split-{}", i); { lhs_add.clone() } <=> { rhs_add.clone() }));
        rules.extend(rw!(format!("mul-split-{}", i); { lhs_mul.clone() } <=> { rhs_mul.clone() }));
        rules.extend(rw!(format!("sub-split-{}", i); { lhs_sub.clone() } <=> { rhs_sub.clone() }));
        rules.push(rw!(format!("neg-split-{}", i); { lhs_neg } => { rhs_neg }));
    }

    rules
}

pub fn rules(vector_width: usize) -> Vec<Rewrite<VecLang, ConstantFold>> {
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![
        rw!("add-0"; "(+ 0 ?a)" => "?a"),
        rw!("add-0-2"; "(+ ?a 0)" => "?a"),
        rw!("mul-0"; "(* 0 ?a)" => "0"),
        rw!("mul-0-2"; "(* ?a 0)" => "0"),
        rw!("mul-1"; "(* 1 ?a)" => "?a"),
        rw!("mul-1-2"; "(* ?a 1)" => "?a"),
    ];

    // Vector rules
    rules.extend(vectorization_rules(vector_width));

    let rotation_rules = rotation_rules(vector_width);
    let operations_rules = operations_rules(vector_width);
    let split_vectors = split_vectors(vector_width);
    rules.extend(rotation_rules);
    rules.extend(operations_rules);
    rules.extend(split_vectors);

    rules.extend(vec![
        //  Basic associativity/commutativity/identities
        // rw!("commute-Add"; "(+ ?a ?b)" => "(+ ?b ?a)"),
        // rw!("commute-Mul"; "(* ?a ?b)" => "(* ?b ?a)"),
        // rw!("assoc-Add"; "(+ (+ ?a ?b) ?c)" => "(+ ?a ( + ?b ?c))"),
        // rw!("assoc-Mul"; "(* ( * ?a ?b) ?c)" => "(* ?a ( * ?b ?c))"),
        // rw!("commute-vecadd"; "(VecAdd ?a ?b)" => "(VecAdd ?b ?a)"),
        // rw!("commute-vecmul"; "(VecMul ?a ?b)" => "(VecMul ?b ?a)"),
        // rw!("assoc-vecadd"; "(VecAdd (VecAdd ?a ?b) ?c)" => "(VecAdd ?a (VecAdd ?b ?c))"),
        // rw!("assoc-vecmul"; "(VecMul (VecMul ?a ?b) ?c)" => "(VecMul ?a (VecMul ?b ?c))"),
    ]);

    rules
}
