use egg::*;
use std::collections::HashMap;
use rand::Rng;
use std::collections::HashSet;
use crate::veclang::VecLang;

pub struct SimulatedAnnealingExtractor<'a, L: Language, N: Analysis<L>> {
    egraph: &'a egg::EGraph<L, N>,
}

impl<'a, L, N> SimulatedAnnealingExtractor<'a, L, N>
where
    L: Language + ToString + std::fmt::Display,
    N: Analysis<L>,
{
    pub fn new(egraph: &'a EGraph<L, N>) -> Self {
        SimulatedAnnealingExtractor { egraph }
    }

    fn causes_cycle(&self, enode: &L, visited: &HashSet<Id>) -> bool {
        // Check if any of the children's eclasses have been visited
        enode.children().iter().any(|child| {
            let child_eclass = self.egraph.find(*child);
            visited.contains(&child_eclass)
        })
    }

    fn calculate_op_cost(op: &str) -> usize {
        // Cost definitions
        const LITERAL: usize = 0;
        const STRUCTURE: usize = 2000;
        const VEC_OP: usize = 1;
        const OP: usize = 1;
    
        // Calculate cost based on operator type
        match op {
            "+" | "*" | "-" | "neg" => OP * 10000,
            "<<" => VEC_OP * 50,
            "Vec" => STRUCTURE,
            "VecAdd" | "VecMinus" => VEC_OP,
            "VecMul" => VEC_OP * 100,
            _ => LITERAL,
        }
    }    

    pub fn extract_expression(
        &mut self,
        expr: &mut RecExpr<L>,
        cost: &mut usize,
        eclass_id: Id,
        visited: &mut HashSet<Id>, // Track visited eclasses
        selected_enodes: &mut HashSet<(L, Id)>,
    ) -> (Id, usize) {
        // RNG setup for selecting a random enode
        let mut rng = rand::thread_rng();

        // Find the eclass in the egraph
        let eclass = self.egraph.find(eclass_id);
        visited.insert(eclass_id); // Mark the eclass as visited

        // Get the enodes for this eclass
        let enodes = &self.egraph[eclass].nodes;
        let total_enodes = enodes.len();

        if total_enodes == 0 {
            panic!("No enodes found for eclass {:?}", eclass); // Or handle as needed
        }

        // Create a vector for valid enodes (those that do not cause cycles)
        let valid_enodes: Vec<&L> = enodes
            .iter()
            .filter(|child| !self.causes_cycle(child, visited))
            .collect();

        let valid_enodes_count = valid_enodes.len();

        if valid_enodes_count == 0 {
            // return Err(format!("No valid enodes found for eclass {eclass:?} that do not cause cycles"));
        }

        // Select a random enode from the valid enodes
        let random_index = rng.gen_range(0..valid_enodes_count);
        let selected_node = valid_enodes[random_index];

        // Get the operator as a string for cost calculation
        let op = selected_node.to_string();
        let op_cost = Self::calculate_op_cost(&op);
        *cost += op_cost;

        // Clone the selected node for manipulation
        let cloned_node = selected_node.clone(); // Clone here to avoid moving
        selected_enodes.insert((cloned_node.clone(), eclass_id));

        // Recursively extract children and build the expression
        let node = cloned_node.map_children(|child| {
            self.extract_expression(expr, cost, child, visited, selected_enodes)
                .0 // Call with the cloned node
        });

        // Remove the eclass from visited once done
        visited.remove(&eclass_id);

        // Add the node to the expression and return
        (expr.add(node), *cost)
    }

    fn generate_neighbor_expression(
        &mut self,
        expr: &mut RecExpr<L>,
        cost: &mut usize,
        eclass_id: Id,
        previous_selected_enodes: HashSet<(L, Id)>,
        selected_enodes: &mut HashSet<(L, Id)>,
        changed: &mut bool,
        visited: &mut HashSet<Id>,
    ) -> (Id, usize) {

        // RNG setup for selecting a random enode
        let mut rng = rand::thread_rng();
        
        let eclass = self.egraph.find(eclass_id);
        visited.insert(eclass_id);

        let enodes = &self.egraph[eclass].nodes;
        let total_enodes = enodes.len();

        if total_enodes == 0 {
            panic!("No enodes found for eclass {:?}", eclass);
        }

        // Create a vector for valid enodes (those that do not cause cycles)
        let valid_enodes: Vec<&L> = enodes
            .iter()
            .filter(|child| !self.causes_cycle(child, visited))
            .collect();

        let valid_enodes_count = valid_enodes.len();

        if valid_enodes_count == 0 {
            panic!("No enodes found for eclass {:?}", eclass);
        }

        let mut selected_node: &L = valid_enodes[0];

        if !*changed {
            if let Some(&(ref prev_enode, _)) = previous_selected_enodes
                .iter()
                .find(|(_, id)| *id == eclass_id)
            {
                let probability_of_change = 0.3;
                if rng.gen_bool(probability_of_change) {
                    let random_index = rng.gen_range(0..valid_enodes_count);
                    selected_node = valid_enodes[random_index];
                    *changed = true;
                } else {
                    selected_node = &prev_enode;
                }
            }
        } else {
            // If already changed, always choose the previous enode, if no enode exists, we choose another one randomly
            if let Some(&(ref prev_enode, _)) = previous_selected_enodes
                .iter()
                .find(|(_, id)| *id == eclass_id)
            {
                selected_node = &prev_enode;
            } else {
                // Select an enode randomly
                let random_index = rng.gen_range(0..valid_enodes_count);
                selected_node = valid_enodes[random_index];
            }
        }

        // Get the operator as a string for cost calculation
        let op = selected_node.to_string();
        let op_cost = Self::calculate_op_cost(&op);
        *cost += op_cost;

        selected_enodes.insert((selected_node.clone(), eclass_id));

        let node = selected_node.clone().map_children(|child| {
            self.generate_neighbor_expression(
                expr,
                cost,
                child,
                previous_selected_enodes.clone(),
                selected_enodes,
                changed,
                visited,
            ).0
        });

        visited.remove(&eclass_id);

        (expr.add(node), *cost)

    }


    pub fn find_best(
        &mut self,
        egraph: &'a EGraph<L, N>,
        initial_eclass_id: Id,
        max_iteration: usize,
        initial_temp: f64,
        cooling_rate: f64
    ) -> (usize, RecExpr<L>){
        let mut rng = rand::thread_rng();
        let mut current_expr = RecExpr::default();
        let mut current_cost = usize::MAX;
        let mut selected_enodes = HashSet::new();
        let mut currently_selected_enodes = HashSet::new();
        let mut best_expr = current_expr.clone();
        let mut best_cost = current_cost;

        let mut temperature = initial_temp;

        // Step1 : Generate the initial expression
        let mut visited = HashSet::new();

        let (_, initial_cost) = self.extract_expression(
            &mut current_expr,
            &mut current_cost,
            initial_eclass_id,
            &mut visited,
            &mut selected_enodes,
        );

        // Update current cost and best cost based on the initial cost
        current_cost = initial_cost;
        best_cost = current_cost;
        best_expr = current_expr.clone();

        // Step2: Iterate to generate neighnor expressions
        for _ in 0..max_iteration {
            let mut neighbor_expr = current_expr.clone();
            let mut neighbor_cost = current_cost.clone();
            let mut changed = false;


            let (_, new_cost) = self.generate_neighbor_expression(
            &mut neighbor_expr,
            &mut neighbor_cost,
            initial_eclass_id, // Assuming the same initial eclass_id for neighbors
            selected_enodes.clone(),
            &mut currently_selected_enodes.clone(),
            &mut changed,
            &mut HashSet::new(), // Visited set for the new expression
        );
            selected_enodes = currently_selected_enodes;
            currently_selected_enodes = HashSet::new();
            // eprintln!("Iteration {}: Extracted Expression: {:?}", i, current_expr);

            if neighbor_cost < current_cost ||  rng.gen::<f64>() < ((current_cost as f64 - new_cost as f64) / temperature).exp() {
                current_cost = neighbor_cost;

                // eprintln!("modified current: {:?}", current_cost);
                if current_cost < best_cost {
                    // eprintln!("modified best:");
                    best_expr = current_expr.clone();
                    best_cost = current_cost;
                }
            }
            
            temperature *= cooling_rate;
        }
        

        (best_cost, best_expr)
    }


}
