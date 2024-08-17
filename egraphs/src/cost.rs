use std::collections::HashMap;

use egg::*;

use crate::veclang::{Egraph, VecLang};

pub struct VecCostFn<'a> {
    pub egraph: &'a Egraph,
}

// &'a EGraph
pub trait CostFunction<L: Language> {
    type Cost;
    fn cost<C>(&mut self, enode: &L, costs: C) -> usize
    where
        C: FnMut(Id) -> usize;

    /// Calculates the total cost of a [`RecExpr`].
    ///
    /// As provided, this just recursively calls `cost` all the way
    /// down the [`RecExpr`].
    ///
    /// [`RecExpr`]: struct.RecExpr.html
    fn cost_rec(&mut self, expr: &RecExpr<L>) -> usize {
        let mut costs: HashMap<Id, usize> = HashMap::default();
        for (i, node) in expr.as_ref().iter().enumerate() {
            let cost = self.cost(node, |i| costs[&i].clone());
            costs.insert(Id::from(i), cost);
        }
        let last_id = Id::from(expr.as_ref().len() - 1);
        costs[&last_id].clone()
    }
}
impl CostFunction<VecLang> for VecCostFn<'_> {
    type Cost = usize;
    // you're passed in an enode whose children are costs instead of eclass ids

    fn cost<C>(&mut self, enode: &VecLang, mut costs: C) -> Self::Cost
    where
        C: FnMut(Id) -> Self::Cost,
    {
        const LITERAL: usize = 0;
        const STRUCTURE: usize = 2000;
        const VEC_OP: usize = 1;

        let op_cost = match enode {
            VecLang::Vec(..) => STRUCTURE,
            VecLang::Rot(..) => VEC_OP * 50,
            VecLang::VecAdd(..) => VEC_OP,
            VecLang::VecMinus(..) => VEC_OP,
            VecLang::VecMul(..) => VEC_OP * 100,
            VecLang::VecNeg(..) => VEC_OP,
            // You get literals for extremely cheap
            _ => LITERAL,
        };
        enode.fold(op_cost, |sum, id| sum + costs(id))
    }
}
