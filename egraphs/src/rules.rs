use crate::{
    binopsearcher::build_binop_or_zero_rule,
    config::*,
    cost::VecCostFn,
    extractor::Extractor,
    searchutils::*,
    veclang::{ConstantFold, Egraph, VecLang},
};
use egg::rewrite as rw;
use egg::*;

// Check if all the variables, in this case memories, are equivalent
fn filter_applicable_rules(
    rules: &mut Vec<Rewrite<VecLang, ConstantFold>>,
    prog: &RecExpr<VecLang>,
) {
    let prog_str: String = prog.pretty(80);
    let ops_to_filter = vec!["neg", "sqrt", "/"];
    let unused_ops: Vec<&&str> = ops_to_filter
        .iter()
        .filter(|&op| !prog_str.contains(op))
        .collect();

    let mut dropped = "".to_string();
    rules.retain(|r| {
        let drop = unused_ops.iter().any(|&op| {
            let rule_sr = format!("{:?}", r);
            rule_sr.contains(op)
        });
        if drop {
            dropped = format!("{} {}", dropped, r.name())
        };
        !drop
    });
    if dropped != "" {
        eprintln!("Dropping inapplicable rules:{}", dropped);
    }
}

/// Run the rewrite rules over the input program and return the best (cost, program)
pub fn run(
    prog: &RecExpr<VecLang>,
    timeout: u64,
    no_ac: bool,
    no_vec: bool,
) -> (f64, RecExpr<VecLang>) {
    let mut rules = rules(no_ac, no_vec);
    filter_applicable_rules(&mut rules, prog);
    let mut init_eg: Egraph = Egraph::new(ConstantFold);
    init_eg.add(VecLang::Num(0));

    let runner = Runner::default()
        .with_egraph(init_eg)
        .with_expr(&prog)
        .with_node_limit(10_000_000)
        .with_time_limit(std::time::Duration::from_secs(timeout))
        .with_iter_limit(10_000)
        .run(&rules);

    // print reason to STDERR.
    // print reason to STDERR.
    eprintln!(
        "Stopped after {} iterations, reason: {:?}",
        runner.iterations.len(),
        runner.stop_reason
    );

    let (eg, root) = (runner.egraph, runner.roots[0]);

    // Always add the literal zero
    let mut extractor = Extractor::new(&eg, VecCostFn { egraph: &eg });
    extractor.find_best(root)
}

pub fn build_binop_rule(op_str: &str, vec_str: &str) -> Rewrite<VecLang, ConstantFold> {
    let searcher: Pattern<VecLang> =
        vec_fold_op(&op_str.to_string(), &"a".to_string(), &"b".to_string())
            .parse()
            .unwrap();

    let applier: Pattern<VecLang> = format!(
        "({} {} {})",
        vec_str,
        vec_with_var(&"a".to_string()),
        vec_with_var(&"b".to_string())
    )
    .parse()
    .unwrap();

    rw!(format!("{}_binop", op_str); { searcher } => { applier })
}

pub fn build_unop_rule(op_str: &str, vec_str: &str) -> Rewrite<VecLang, ConstantFold> {
    let searcher: Pattern<VecLang> = vec_map_op(&op_str.to_string(), &"a".to_string())
        .parse()
        .unwrap();
    let applier: Pattern<VecLang> = format!("({} {})", vec_str, vec_with_var(&"a".to_string()))
        .parse()
        .unwrap();

    rw!(format!("{}_unop", op_str); { searcher } => { applier })
}

pub fn rotation_rules() -> Vec<Rewrite<VecLang, ConstantFold>> {
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![];
    // let searcher_combine: Pattern<VecLang> = "( << ( << ?a ?b ) ?c)".parse().unwrap();
    // let applier_combine: Pattern<VecLang> = " (<< ?a ( + ?b ?c) )".parse().unwrap();
    // let rule_combine: Vec<Rewrite<VecLang, ConstantFold>> =
    //     rw!("rotation_combine" ; {searcher_combine.clone()} <=> {applier_combine.clone()});
    // rules.extend(rule_combine);
    let vector_width: usize = vector_width(); // Store vector width in a constant

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

pub fn split_vectors() -> Vec<Rewrite<VecLang, ConstantFold>> {
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![];

    let vector_width: usize = vector_width(); // Store vector width in a constant

    let lhs = format!(
        "(Vec {})",
        (0..vector_width)
            .map(|i| format!("?a{} ", i))
            .collect::<String>()
    );
    let searcher: Pattern<VecLang> = lhs.parse().unwrap();

    for i in 0..vector_width {
        let vector1 = format!(
            "(Vec {})",
            (0..vector_width)
                .map(|j| if i == j {
                    "0 ".to_string()
                } else {
                    format!("?a{} ", j)
                })
                .collect::<String>()
        );

        let vector2 = format!(
            "(Vec {})",
            (0..vector_width)
                .map(|j| if i == j {
                    format!("?a{} ", j)
                } else {
                    "0 ".to_string()
                })
                .collect::<String>()
        );

        let rhs = format!("(VecAdd {} {})", vector1, vector2);
        let applier: Pattern<VecLang> = rhs.parse().unwrap();
        let rule: Vec<Rewrite<VecLang, ConstantFold>> =
            rw!(format!("split-{}", i); { searcher.clone() } <=> { applier.clone() });
        rules.extend(rule);
    }

    rules
}

pub fn operations_rules() -> Vec<Rewrite<VecLang, ConstantFold>> {
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![];

    let vector_width: usize = vector_width(); // Store vector width in a constant

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
                vector_neg.push(format!("( neg ?a{}) ", j));
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
                vector1.push(format!("?a{}1 ", j));
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
        rules.push(rw!(format!("add-split-{}", i); { lhs_add } => { rhs_add }));
        rules.push(rw!(format!("mul-split-{}", i); { lhs_mul } => { rhs_mul }));
        rules.push(rw!(format!("sub-split-{}", i); { lhs_sub } => { rhs_sub }));
        rules.push(rw!(format!("neg-split-{}", i); { lhs_neg } => { rhs_neg }));
    }

    rules
}

pub fn rules(no_ac: bool, no_vec: bool) -> Vec<Rewrite<VecLang, ConstantFold>> {
    let mut rules: Vec<Rewrite<VecLang, ConstantFold>> = vec![
        rw!("add-0"; "(+ 0 ?a)" => "?a"),
        rw!("mul-0"; "(* 0 ?a)" => "0"),
        rw!("mul-1"; "(* 1 ?a)" => "?a"),
    ];

    // let rotation_rules = rotation_rules();
    // let operations_rules = operations_rules();
    // let split_vectors = split_vectors();
    // rules.extend(rotation_rules);
    // rules.extend(operations_rules);
    // rules.extend(split_vectors);

    // Vector rules
    if !no_vec {
        rules.extend(vec![
            // Special MAC fusion rule

            // Custom searchers
            build_unop_rule("-", "VecNeg"),
            build_binop_or_zero_rule("+", "VecAdd"),
            build_binop_or_zero_rule("*", "VecMul"),
            build_binop_or_zero_rule("-", "VecMinus"),
        ]);
    } else {
        eprintln!("Skipping vector rules")
    }

    if !no_ac {
        rules.extend(vec![
            //  Basic associativity/commutativity/identities
            rw!("commute-vecadd"; "(VecAdd ?a ?b)" => "(VecAdd ?b ?a)"),
            rw!("commute-vecmul"; "(VecMul ?a ?b)" => "(VecMul ?b ?a)"),
            rw!("assoc-vecadd"; "(VecAdd (VecAdd ?a ?b) ?c)" => "(VecAdd ?a (VecAdd ?b ?c))"),
            rw!("assoc-vecmul"; "(VecMul (VecMul ?a ?b) ?c)" => "(VecMul ?a (VecMul ?b ?c))"),
        ]);
    }

    rules
}