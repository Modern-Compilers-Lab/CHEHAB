use std::collections::HashMap;

use egg::*;
use log::debug;

use crate::veclang::{Egraph, VecLang};

pub struct VecCostFn<'a> { 
    pub egraph: &'a Egraph,
}

// &'a EGraph
pub trait CostFunction<L: Language> {
    type Cost;

    // fn cost<C>(&mut self, enode: &L, costs: C) -> usize
    // where
    //     C: FnMut(Id) -> usize;
    
    fn depth<C>(&mut self, enode: &L, costs: C) -> f64
    where
        C: FnMut(Id) -> f64;

    fn mul_depth<C>(&mut self, enode: &L, costs: C) -> f64
    where
        C: FnMut(Id) -> f64;

    fn rotations<C>(&mut self, enode: &L, costs: C) -> f64
    where
        C: FnMut(Id) -> f64;

    fn operations_cost<C>(&mut self, enode: &L, costs: C) -> f64
    where
        C: FnMut(Id) ->f64;

    /// Calculates the total cost of a [`RecExpr`].
    ///
    /// As provided, this just recursively calls `cost` all the way
    /// down the [`RecExpr`].
    ///
    /// [`RecExpr`]: struct.RecExpr.html

    fn cost_rec(&mut self, expr: &RecExpr<L>) -> f64 {
        let mut costs: HashMap<Id, f64> = HashMap::default();
        for (i, node) in expr.as_ref().iter().enumerate() {
            let cost = self.operations_cost(node, |i| costs[&i].clone());
            costs.insert(Id::from(i), cost);
        }
        let last_id = Id::from(expr.as_ref().len() - 1);
        costs[&last_id].clone()
    }
}
impl CostFunction<VecLang> for VecCostFn<'_> {
    type Cost = f64;
    // you're passed in an enode whose children are costs instead of eclass ids

    fn depth<C>(&mut self, enode: &VecLang, mut costs: C) -> Self::Cost
    where
        C: FnMut(Id) -> Self::Cost,
    {
        let op_cost = match enode {
            VecLang::Num(..) | VecLang::Symbol(..) => 0.0,
            _ => 1.0,
        };

        if matches!(
            enode,
            VecLang::VecAddRotF(..)
                | VecLang::VecAddRotP(..)
                | VecLang::VecMinusRotF(..)
                | VecLang::VecMinusRotP(..)
                | VecLang::VecMulRotF(..)
                | VecLang::VecMulRotP(..)
        ) {
            if let Some(rotation_node) = enode.children().last() {
                let eclass = &self.egraph[*rotation_node];
                if let Some(node) = eclass.nodes.get(0) {
                    if let VecLang::Num(value) = node {
                        let num_rotations = *value as f64;
                        debug!("Rotation value: {:?}", num_rotations);
                                
                        let rotation_cost = num_rotations * 2.0;
                        let children_cost = enode.fold(0.0, |max: f64, id| max.max(costs(id)));
                        return rotation_cost + children_cost;
                    }
                }
            }
        }
        
        op_cost + enode.fold(0.0, |max: f64, id| max.max(costs(id)))
    }

    fn mul_depth<C>(&mut self, enode: &VecLang, mut costs: C) -> Self::Cost
    where
        C: FnMut(Id) -> Self::Cost,
    {
        let op_cost = match enode {
            VecLang::VecMul(..) | VecLang::VecMulRotF(..) | VecLang::VecMulRotP(..) | VecLang::Mul(..) => 1.0,
            _ => 0.0,
        };
        
        if matches!(
            enode,
                VecLang::VecMulRotF(..) | VecLang::VecMulRotP(..)
        ) {
            if let Some(rotation_node) = enode.children().last() {
                let eclass = &self.egraph[*rotation_node];
                if let Some(node) = eclass.nodes.get(0) {
                    if let VecLang::Num(value) = node {
                        let num_rotations = *value as f64;
                        debug!("Rotation value: {:?}", num_rotations);
                                
                        let rotation_cost = num_rotations * 2.0;
                        let children_cost = enode.fold(0.0, |max: f64, id| max.max(costs(id)));
                        return rotation_cost + children_cost;
                    }
                }
            }
        }
        
        op_cost + enode.fold(0.0, |max: f64, id| max.max(costs(id)))    }

    fn rotations<C>(&mut self, enode: &VecLang, mut costs: C) -> Self::Cost
    where
        C: FnMut(Id) -> Self::Cost,
    {
        // Assign a base cost for specific operations
        let op_cost = match enode {
            VecLang::VecAddRotF(..) | VecLang::VecAddRotP(..) |
            VecLang::VecMinusRotF(..) | VecLang::VecMinusRotP(..) |
            VecLang::VecMulRotF(..) | VecLang::VecMulRotP(..) => 1.0,
            _ => 0.0,   //rotations cost 
        };
    
        if matches!(
            enode,
            VecLang::VecAddRotF(..)
                | VecLang::VecAddRotP(..)
                | VecLang::VecMinusRotF(..)
                | VecLang::VecMinusRotP(..)
                | VecLang::VecMulRotF(..)
                | VecLang::VecMulRotP(..)
        ) {
            if let Some(rotation_node) = enode.children().last() {
                let eclass = &self.egraph[*rotation_node];
                if let Some(node) = eclass.nodes.get(0) {
                    if let VecLang::Num(value) = node {
                        let num_rotations = *value as f64;
                        debug!("Rotation value: {:?}", num_rotations);
    
                        // Compute the rotation cost
                        let rotation_cost = op_cost * num_rotations;
                        let children_cost = enode.fold(0.0, |sum, id| sum + costs(id));
                        return rotation_cost + children_cost;

                    } else {
                        eprintln!("Node is not a Num variant");
                    }
                } else {
                    eprintln!("No nodes found in eclass");
                }
            }
        }
    
        op_cost + enode.fold(0.0, |sum, id| sum + costs(id))
    }
    


    fn operations_cost<C>(&mut self, enode: &VecLang, mut costs: C) -> Self::Cost
    where
        C: FnMut(Id) -> Self::Cost,
    {

        const LITERAL: usize = 0;
        const STRUCTURE: usize = 2000;
        const VEC_OP: usize = 1;
        const OP: usize = 1;

         let op_cost = match enode {
            // You get literals for extremely cheap
            // VecLang::Num(value) => (*value * 4_000) as usize,
            VecLang::Num(..) => LITERAL,
            VecLang::Symbol(..) => LITERAL,

            // Vectors are cheap if they have literal values
            VecLang::Vec(..) => STRUCTURE,

            // But scalar and vector ops cost something
            VecLang::Add(..) => OP * 10_000,
            VecLang::Mul(..) => OP * 10_000,
            VecLang::Minus(..) => OP * 10_000,
            VecLang::Neg(..) => 10_000 * OP,
            
            VecLang::Rot(..) => VEC_OP * 50,
            VecLang::VecAdd(..) => VEC_OP,
            VecLang::VecMinus(..) => VEC_OP,
            VecLang::VecMul(..) => VEC_OP * 100,

            VecLang::VecAddRotF(..) | VecLang::VecAddRotS(..) => VEC_OP * 1051,
            VecLang::VecMinusRotF(..) | VecLang::VecMinusRotS(..) => VEC_OP * 1051,
            VecLang::VecMulRotF(..) | VecLang::VecMulRotS(..) => VEC_OP * 2150,

            VecLang::VecAddRotP(..) => VEC_OP * 5_000,
            VecLang::VecMinusRotP(..) => VEC_OP * 5_000,
            VecLang::VecMulRotP(..) => VEC_OP * 7_000,

            VecLang::VecNeg(..) => VEC_OP,
        };

        enode.fold(op_cost as f64, |sum, id| sum + costs(id))
    }

}