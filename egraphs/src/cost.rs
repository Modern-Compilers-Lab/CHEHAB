use std::collections::HashMap;

use egg::*;

use crate::config::*;
use crate::fhelang::{Egraph, FheLang};
pub struct CostFn<'a> {
    pub egraph: &'a Egraph,
}

// &'a EGraph
pub trait CostFunction<L: Language> {
    fn mul_depth<C>(&mut self, enode: &L, costs: C) -> f64
    where
        C: FnMut(Id) -> f64;
    fn depth<C>(&mut self, enode: &L, costs: C) -> f64
    where
        C: FnMut(Id) -> f64;
    type Cost;
    fn op_costs<C>(&mut self, enode: &L, costs: C) -> f64
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
            let cost = self.op_costs(node, |i| costs[&i].clone());
            costs.insert(Id::from(i), cost);
        }
        let last_id = Id::from(expr.as_ref().len() - 1);
        costs[&last_id].clone()
    }
}
impl CostFunction<FheLang> for CostFn<'_> {
    type Cost = f64;
    // you're passed in an enode whose children are costs instead of eclass ids
    fn depth<C>(&mut self, enode: &FheLang, mut costs: C) -> Self::Cost
    where
        C: FnMut(Id) -> Self::Cost,
    {
        let op_cost = match enode {
            FheLang::Constant(..) | FheLang::Symbol(..) => 0.0,
            _ => 1.0,
        };
        op_cost + enode.fold(0.0, |max: f64, id| max.max(costs(id)))
    }
    fn mul_depth<C>(&mut self, enode: &FheLang, mut costs: C) -> Self::Cost
    where
        C: FnMut(Id) -> Self::Cost,
    {
        let op_cost = match enode {
            FheLang::Mul(..) => 1.0,
            FheLang::Square(..) => 1.0,
            _ => 0.0,
        };
        op_cost + enode.fold(0.0, |max: f64, id| max.max(costs(id)))
    }
    fn op_costs<C>(&mut self, enode: &FheLang, mut costs: C) -> Self::Cost
    where
        C: FnMut(Id) -> Self::Cost,
    {
        let op_cost = match enode {
            FheLang::Add(..) => OP,
            FheLang::Minus(..) => OP,
            FheLang::Neg(..) => OP,
            FheLang::Mul(..) => OP * 100.0,
            FheLang::Rot(..) => OP * 50.0,
            FheLang::Square(..) => OP * 80.0,
            _ => LITERAL,
        };
        op_cost + enode.fold(0.0, |sum, id| sum + costs(id))
    }
}
