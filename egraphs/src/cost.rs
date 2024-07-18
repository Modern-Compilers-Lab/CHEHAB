use std::collections::HashMap;

use egg::*;

use crate::veclang::{Egraph, VecLang};

pub struct VecCostFn<'a> {
    pub egraph: &'a Egraph,
}

// &'a EGraph
pub trait CostFunction<L: Language> {
    type Cost;
    fn cost<C>(&mut self, enode: &L, costs: C) -> f64
    where
        C: FnMut(Id) -> f64;

    /// Calculates the total cost of a [`RecExpr`].
    ///
    /// As provided, this just recursively calls `cost` all the way
    /// down the [`RecExpr`].
    ///
    /// [`RecExpr`]: struct.RecExpr.html
    fn cost_rec(&mut self, expr: &RecExpr<L>) -> f64 {
        let mut costs: HashMap<Id, f64> = HashMap::default();
        for (i, node) in expr.as_ref().iter().enumerate() {
            let cost = self.cost(node, |i| costs[&i].clone());
            costs.insert(Id::from(i), cost);
        }
        let last_id = Id::from(expr.as_ref().len() - 1);
        costs[&last_id].clone()
    }
}
impl CostFunction<VecLang> for VecCostFn<'_> {
    type Cost = f64;
    // you're passed in an enode whose children are costs instead of eclass ids

    fn cost<C>(&mut self, enode: &VecLang, mut costs: C) -> Self::Cost
    where
        C: FnMut(Id) -> Self::Cost,
    {
        const LITERAL: f64 = 0.001;
        const STRUCTURE: f64 = 2000.0;
        const VEC_OP: f64 = 1.;
        const OP: f64 = 1.;

        let op_cost = match enode {
            // You get literals for extremely cheap
            VecLang::Num(..) => LITERAL,
            VecLang::Symbol(..) => LITERAL,

            // Vectors are cheap if they have literal values
            VecLang::Vec(..) => STRUCTURE,

            // But scalar and vector ops cost something
            VecLang::Add(vals) => OP * 10000.0 * (vals.iter().count() as f64 - 1.),
            VecLang::Mul(vals) => OP * 10000.0 * (vals.iter().count() as f64 - 1.),
            VecLang::Minus(vals) => OP * 10000.0 * (vals.iter().count() as f64 - 1.),

            VecLang::Neg(..) => 10000.0 * OP,

            VecLang::Rot(..) => VEC_OP * 10.,
            VecLang::VecAdd(..) => VEC_OP,
            VecLang::VecMinus(..) => VEC_OP,
            VecLang::VecMul(..) => VEC_OP * 100.0,

            VecLang::VecNeg(..) => VEC_OP,
        };
        enode.fold(op_cost, |sum, id| sum + costs(id))
    }
}
