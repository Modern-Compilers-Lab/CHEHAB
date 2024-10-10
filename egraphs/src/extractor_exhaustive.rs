use egg::*;
use std::collections::{HashMap, HashSet};

use crate::config::*;
pub struct Extractor<'a, L: Language, N: Analysis<L>> {
    egraph: &'a egg::EGraph<L, N>,
}

impl<'a, L, N> Extractor<'a, L, N>
where
    L: Language + ToString  + std::fmt::Display,
    N: Analysis<L>,
{
    pub fn new(egraph: &'a EGraph<L, N>) -> Self {
        let extractor = Extractor { egraph };
        extractor
    }
    // The find_best function tests all possible combinations of e-nodes within an e-graph,
    // aiming to find the overall optimal expression based on the total cost, rather than selecting the
    // best node locally within each e-class. This approach helps in avoiding suboptimal expressions that
    // might arise from local optimization. The function considers the following parameters:
    //
    // - self: A mutable reference to the extractor object, which contains the egraph in question.
    // - eclass_ids: A vector storing the e-classes to traverse, based on the previously selected e-nodes.
    // - dependency_map: A HashMap that tracks dependencies among e-classes, helping in cycle detection.
    //   A cycle (when an e-class has itself as a dependency) would result in an invalid combination.
    // - root_id: The ID of the e-class that represents the entire expression.
    // - current_index: The index of the current e-class being processed in the eclass_ids vector.
    // - current_cost: The cost accumulated so far from the already chosen nodes.
    // - current_nodes: A vector storing the nodes that are currently considered the best nodes.
    // - best_cost: A mutable reference to store the cost of the best expression found so far.
    // - best_expr: A mutable reference to store the best expression found so far.
    //
    // The function starts by retrieving the current e-class using the provided index. It then iterates
    // over the nodes of this e-class. For each node, the function clones the current state, calculates
    // the cost of the operation, updates dependencies, and recursively explores further combinations
    // unless a cycle is detected or a scalar operation is encountered (which is skipped for vectorization).
    // Once the last e-class is processed, the function compares the current expression's cost with the
    // best cost found so far. If the current expression is cheaper, it updates the best cost and the
    // best expression. The nodes in the best expression are stored in a bottom-up manner, requiring
    // special handling to adjust the IDs of the children.
    pub fn find_best(
        &mut self,
        eclass_ids: Vec<Id>,
        dependency_map: HashMap<Id, HashSet<Id>>,
        root_id: Id,
        current_index: usize,
        current_cost: usize,
        current_nodes: Vec<L>,
        best_cost: &mut usize,
        best_expr: &mut RecExpr<L>,
        memo: &mut HashMap<(Id, Vec<Id>), (usize, RecExpr<L>)>,
    ) {
        let current_eclass = &self.egraph[eclass_ids[current_index]];

        // Convert the dependency map into a sorted vector to ensure hashing consistency
        let mut dep_vec: Vec<_> = dependency_map
            .iter()
            .flat_map(|(_, deps)| deps.iter().cloned())
            .collect();
        dep_vec.sort();

        // check if the current combination of e-classes and dependencies has already been computed
        if let Some((cached_cost, cached_expr)) = memo.get(&(current_eclass.id, dep_vec.clone())) {
            if *cached_cost < *best_cost {
                *best_cost = *cached_cost;
                *best_expr = cached_expr.clone();
            }
            return; // Reuse the cached result
        }

        'node_loop: for node in &current_eclass.nodes {
            // Create new variables for each iteration to avoid unnecessary mutation of the originals
            let mut next_eclass_ids = eclass_ids.clone(); // Clone eclass_ids to preserve the original state
            let mut next_dependency_map = dependency_map.clone(); // Clone dependency_map to track dependencies in this iteration
            let mut next_nodes = current_nodes.clone(); // Clone current_nodes to build the expression incrementally
            let mut next_cost = current_cost; // Copy the current cost to update it for this iteration

            // Determine the operation type of the current node
            let operation = node.to_string();
            match operation.as_str() {
                // Skip scalar operations since they are not useful for vectorization
                // "+" | "*" | "-" => continue 'node_loop,

                // Add specific costs for vector operations and structures
                "<<" => next_cost += VEC_OP * 50, // Cost for vector shift operation
                "Vec" => next_cost += STRUCTURE,  // Cost for vector structure
                "VecAdd" | "VecMinus" | "VecNeg" => next_cost += VEC_OP, // Cost for basic vector operations
                "VecMul" => next_cost += VEC_OP * 100, // Higher cost for vector multiplication
                "+" | "*" | "-" => next_cost += VEC_OP * 10000,

                // Default cost for any other operations (assumed to be literal)
                _ => next_cost += LITERAL,
            }

            // Initialize a set to store the dependencies (children IDs) of the current node
            let mut dependent_ids: HashSet<Id> = HashSet::new();
            let node = node.clone(); // Clone the current node to avoid mutating the original

            // Map the children of the current node to their positions in the e-class IDs vector
            let mapped_node = node.map_children(|child_id| {
                // Insert the child ID into the set of dependencies
                dependent_ids.insert(child_id);
                dependent_ids = dependent_ids
                    .union(
                        next_dependency_map
                            .get(&child_id)
                            .unwrap_or(&HashSet::new()),
                    )
                    .cloned()
                    .collect();
                // Check if the child ID is already in the list of e-class IDs
                match next_eclass_ids.iter().position(|&id| id == child_id) {
                    // If found, return its position
                    Some(pos) => pos.into(),
                    // If not found, add the child ID to the list and return its new position
                    None => {
                        next_eclass_ids.push(child_id);
                        (next_eclass_ids.len() - 1).into()
                    }
                }
            });

            next_dependency_map.insert(current_eclass.id, dependent_ids.clone());

            // Update dependencies of other e-classes based on the current e-class
            let mut updated_map = next_dependency_map.clone();
            for (&key, deps) in &next_dependency_map {
                // Check if the current e-class is a dependency of any other e-class
                if deps.contains(&current_eclass.id) {
                    // Merge the dependencies of the current e-class with the existing dependencies
                    let merged_deps: HashSet<Id> = deps.union(&dependent_ids).cloned().collect();
                    // If the merged dependencies contain the key itself, it indicates a cycle, so skip this node
                    if merged_deps.contains(&key) {
                        continue 'node_loop; // Skip to avoid cycles
                    }
                    // Otherwise, update the dependency map with the merged dependencies
                    updated_map.insert(key, merged_deps);
                }
            }
            next_dependency_map = updated_map;
            next_nodes.push(mapped_node);

            if next_cost >= *best_cost {
                continue 'node_loop;
            }

            if current_index < next_eclass_ids.len() - 1 {
                // Recursively try all combinations with the next eclass
                self.find_best(
                    next_eclass_ids,
                    next_dependency_map,
                    root_id,
                    current_index + 1,
                    next_cost,
                    next_nodes,
                    best_cost,
                    best_expr,
                    memo,
                );
            } else {
                // If the current expression's cost is lower than the best cost found so far, update the best cost

                if next_cost < *best_cost {
                    *best_cost = next_cost;
                    *best_expr = RecExpr::default(); // Reset the best expression to start building the new one

                    let total_nodes = next_nodes.len();
                    // Iterate through the nodes in reverse order to build the expression bottom-up

                    for node in next_nodes.iter().rev() {
                        let node = node.clone();
                        // Adjust the IDs of the children to reflect the reversed order
                        let new_node =
                            node.map_children(|id| (total_nodes - usize::from(id) - 1).into());
                        best_expr.add(new_node); // Add the adjusted node to the best expression
                    }
                }
            }
        }
    }
}