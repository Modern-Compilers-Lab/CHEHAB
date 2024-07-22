use crate::{
    cost::CostFn,
    extractor::Extractor,
    fhe_rules::*,
    fhelang::{ConstantFold, Egraph, FheLang},
};
use egg::*;
pub fn run(
    prog: &RecExpr<FheLang>,
    timeout: u64,
    slot_count: usize,
    axiomatic: bool,
) -> (f64, RecExpr<FheLang>) {
    let rules = rules(slot_count, axiomatic);
    let mut init_eg: Egraph = Egraph::new(ConstantFold);
    init_eg.add(FheLang::Constant(0));

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
    let mut extractor = Extractor::new(&eg, CostFn { egraph: &eg });
    extractor.find_best(root)
}
pub fn rules(slot_count: usize, axiomatic: bool) -> Vec<Rewrite<FheLang, ConstantFold>> {
    let mut rules: Vec<Rewrite<FheLang, ConstantFold>> = vec![];

    if axiomatic {
        rules.extend(axiomatic_rules(slot_count));
    } else {
        rules.extend(specific_rules(slot_count));
    }

    rules
}
