use egg::*;
use std::{
    cmp::Ordering,
    collections::{HashMap, HashSet},
};
use std::time::Instant;

use crate::cost;
pub struct Extractor<'a, CF: cost::CostFunction<L>, L: Language, N: Analysis<L>> {
    cost_function: CF,
    costs: HashMap<Id, (usize, L)>,
    egraph: &'a egg::EGraph<L, N>,
}

impl<'a, CF, L, N> Extractor<'a, CF, L, N>
where
    CF: cost::CostFunction<L>,
    L: Language + ToString  + std::fmt::Display,
    N: Analysis<L>,
{


    pub fn new(
        egraph: &'a EGraph<L, N>,
        cost_function: CF,
        root: Id,
        ) -> Self
    {
            let costs = HashMap::default();
            let mut extractor = Extractor {
                costs,
                egraph,
                cost_function,
            };
            extractor.find_costs();
            extractor
    }

    /// Find the cheapest (lowest cost) represented `RecExpr` in the
    /// given eclass.
    pub fn find_best(&mut self, eclass: Id) -> (usize, RecExpr<L>) {
        let mut expr = RecExpr::default();
        let (_, cost) = self.find_best_rec(&mut expr, eclass);
        (cost, expr)
    }


    fn find_best_rec(&mut self, expr: &mut RecExpr<L>, eclass: Id) -> (Id, usize) {
        let id = self.egraph.find(eclass);
    
       
        let (best_cost, best_node) = match self.costs.get(&id) {
            Some(result) => {
                result.clone()
            }
            None => {
                panic!("Failed to extract from eclass {}", id);
            }  
        };
    
        // Map children and extract recursively
        let node = best_node.map_children(|child| self.find_best_rec(expr, child).0);
    
        // Return the new node and the best cost
        (expr.add(node), best_cost)
    }
    

    /// Calculates the total cost of a node within an e-graph.
    ///
    /// This function calculates the cost of a given node by considering the costs of its child e-classes.
    /// The calculation ensures that if an e-class appears more than once in the hierarchy of the e-node,
    /// it is counted only once.
    ///
    /// Steps:
    /// 1. Ensure all children of the node have their costs already calculated.
    /// 2. If there is only one child, return its cost directly.
    /// 3. If there are two children and they are the same (binary operation with the same operand),
    ///    return the cost minus the cost of one of the children.
    /// 4. If one of the children e-classes belongs to the hierarchy of the other, keep the cost of the container.
    /// 5. Otherwise, calculate the intersection of both hierarchies and subtract the cost of the shared operations.
    ///
    /// The function handles different operations with specific costs to ensure accurate calculation.
    ///
    /// Parameters:
    /// - `node`: The node for which the cost is to be calculated.
    /// - `map`: A mapping of e-class IDs to their respective sub-classes.
    ///
    /// Returns:
    /// - `Some(usize)`: The total cost of the node if all child costs are available.
    /// - `None`: If the cost of the node cannot be calculated due to missing child costs.
    fn node_total_cost(&mut self, node: &L, map: &mut HashMap<Id, HashSet<Id>>) -> Option<usize> {
        let eg = &self.egraph;
    
        // Check if all children have their costs calculated
        let has_cost = |&id| self.costs.contains_key(&eg.find(id));
        if node.children().iter().all(has_cost) {
            let costs = &self.costs;
            let cost_f = |id| costs[&eg.find(id)].0.clone();
            let mut cost = self.cost_function.cost(&node, cost_f);
            let children = node.children();
    
            if children.len() == 1 {
                return Some(cost);
            }
    
            let mut shared_sub_classes: HashSet<Id> = HashSet::new();
    
            // Compare all pairs of children
            for (i, &id_i) in children.iter().enumerate() {
                for &id_j in children.iter().skip(i + 1) {
    
                    // If both children are the same, subtract the cost of one child
                    if id_i == id_j {
                        return Some(cost - costs[&eg.find(id_i)].0);
                    }
    
                    let sub_classes_i = map.get(&id_i).unwrap();
                    let sub_classes_j = map.get(&id_j).unwrap();
    
                    // If one child belongs to the hierarchy of the other, subtract the contained class cost
                    if sub_classes_i.contains(&id_j) {
                        return Some(cost - costs[&eg.find(id_j)].0);
                    }
                    if sub_classes_j.contains(&id_i) {
                        return Some(cost - costs[&eg.find(id_i)].0);
                    }
    
                    // Calculate the intersection of both hierarchies and subtract the cost of shared operations
                    let shared = sub_classes_i
                        .intersection(sub_classes_j)
                        .cloned()
                        .collect::<HashSet<Id>>();
    
                    shared_sub_classes = shared_sub_classes.union(&shared).cloned().collect();
                }
            }
    
            // Adjust the cost based on shared sub-classes
            for id in shared_sub_classes {
                let node = costs[&eg.find(id)].1.clone();
                let op = node.to_string();
    
                // Define operation costs
                const LITERAL: usize = 0;
                const STRUCTURE: usize = 2000;
                const VEC_OP: usize = 1;
                const OP: usize = 1;
    
                let op_costs: usize = match op.as_str() {
                    "+" | "*" | "-" | "neg" => OP * 10000,
                    "<<" => VEC_OP * 50,
                    "Vec" => STRUCTURE,
                    "VecAdd" | "VecMinus" => VEC_OP,
                    "VecMul" => VEC_OP * 100,
                    _ => LITERAL,
                };
    
                cost -= op_costs;
            }
    
            return Some(cost);
        }
    
        None
    }
    

    /// Calculates the costs of all e-classes in an e-graph.
    ///
    /// This function iterates through all e-classes in the e-graph and calculates the cost for each one using
    ///  `make_pass` function. The cost calculation for each e-class considers the costs
    /// of its e-nodes. If the cost of an e-class is calculated for the first time, it is set along with the
    /// corresponding best e-node. The function continues iterating until no changes are detected, ensuring that
    /// all dependent e-classes have their costs updated appropriately.
    ///
    /// Steps:
    /// 1. Initialize a flag (`did_something`) to track if any costs were updated.
    /// 2. Initialize a map (`sub_classes`) to store sub-classes for each e-class.
    /// 3. Iterate over all e-classes and calculate their costs.
    /// 4. If the cost of an e-class is calculated for the first time, update the cost and set the flag to `true`.
    /// 5. If the cost of an e-class is already calculated, update it only if there is a change and set the flag accordingly.
    /// 6. Repeat until no more changes are detected.
    /// 7. Log an error message for any e-class that failed to compute a cost.
    ///
    /// Parameters:
    /// - `&mut self`: Mutable reference to the current instance.
    ///
    /// Returns:
    /// - None
    fn find_costs(&mut self) {
        let mut did_something = true;
        let mut sub_classes: HashMap<Id, HashSet<Id>> = HashMap::new();
        let mut i = 0;

        // let mut enode_descendents : HashMap<(L, Id), HashSet<Id>> = Default::default();
        // enode_descendents = Self::find_enode_descendents(self.egraph);

        // Iterate until no more changes are detected
        while did_something {
            // Start timer for this iteration
            let start_time = Instant::now();

            did_something = false;
            i += 1;

            for class in self.egraph.classes() {
                let pass = self.make_pass(&mut sub_classes, class);
                match (self.costs.get(&class.id), pass) {
                    // If the cost is calculated for the first time
                    (None, Some(new)) => {
                        self.costs.insert(class.id, new);
                        did_something = true;
                    }
                    // If the cost is already calculated and there is a change
                    (Some(old), Some(new)) => {
                        if new.0 != old.0 {
                            self.costs.insert(class.id, new);
                            did_something = true;
                        }
                    }
                    _ => (),
                }
            }

            // Measure the time for the current iteration
            let duration = start_time.elapsed();
            eprintln!("Iteration {} took {:?}", i, duration);
        }

        eprintln!("Total number of iterations: {}", i);

        // Log an error message for any e-class that failed to compute a cost
        for class in self.egraph.classes() {
            if !self.costs.contains_key(&class.id) {
                eprintln!(
                    "Failed to compute cost for eclass {}: {:?}",
                    class.id, class.nodes
                );
            }
        }
    }

    fn cmp(a: &Option<usize>, b: &Option<usize>) -> Ordering {
        match (a, b) {
            (None, None) => Ordering::Equal,
            (None, Some(_)) => Ordering::Greater,
            (Some(_), None) => Ordering::Less,
            (Some(a), Some(b)) => a.partial_cmp(&b).unwrap(),
        }
    }
    /// Calculates the cost of an e-class and determines the best e-node within it.
    ///
    /// This function iterates through all e-nodes in the given e-class to calculate their costs using the
    /// `node_total_cost` function. The cost of the e-class is determined to be the cost of the e-node with the
    /// minimal cost. Once the best e-node is found, the hierarchy of the e-class is set to that of this e-node.
    ///
    /// Parameters:
    /// - `sub_classes`: A mutable reference to a map of e-class IDs to their respective sub-class IDs.
    /// - `eclass`: A reference to the e-class for which the cost is to be calculated.
    ///
    /// Returns:
    /// - `Some((usize, L))`: A tuple containing the minimum cost and the corresponding best e-node.
    /// - `None`: If no valid cost could be calculated for any e-node within the e-class.

    fn make_pass(
        &mut self,
        sub_classes: &mut HashMap<Id, HashSet<Id>>,
        eclass: &EClass<L, N::Data>,
        // enode_descendents: HashMap<(L, Id), HashSet<Id>>,
    ) -> Option<(usize, L)> {
        // Record the start time for the entire function
        let start_time = Instant::now();

        let mut node_sub_classes: HashSet<Id> = HashSet::new();
        let mut nodes: Vec<L> = vec![];

        // Time the node filtering process
        for node in eclass.iter() {
            let op = node.to_string();
            match op.as_str() {
                // "+" | "*" | "-" | "neg" => continue,
                _ => nodes.push(node.clone()),
            }
        }
       

        if nodes.is_empty() {
            println!("No valid nodes found, total time: {:?}", start_time.elapsed());
            return None;
        }

        // Time the cost calculation process
        let cost_calculation_start = Instant::now();
        let (cost, node) = nodes
            .iter()
            .map(|n| (self.node_total_cost(n, sub_classes), n))
            .min_by(|a, b| Self::cmp(&a.0, &b.0))
            .unwrap();
        
        
        let cost_calculation_duration = cost_calculation_start.elapsed();
        // eprintln!("Cost calculation took: {:?}", cost_calculation_duration);

        match cost {
            // If no valid cost could be calculated, return None
            None => {
                return None;
            },

            // If a valid cost is found
            Some(cost) => {

                node.for_each(|id| {
                    node_sub_classes.insert(id);

                    if let Some(sub_class_set) = sub_classes.get(&id) {
                        node_sub_classes.extend(sub_class_set.iter());
                    }
                });

                sub_classes.insert(eclass.id, node_sub_classes);

               

                // Print the total time taken for the function
                // eprintln!("Total time for make_pass: {:?}", start_time.elapsed());

                Some((cost, node.clone()))
            }
        }
    }

    fn find_enode_descendents(
        egraph: &'a EGraph<L, N>,
    ) -> HashMap<(L, Id), HashSet<Id>>
    {
        let mut enode_descendents: HashMap<(L, Id), HashSet<Id>> = HashMap::new();
        let mut visited: HashSet<Id> = HashSet::new();

        for class in egraph.classes() {
            for node in &class.nodes {
                let mut result_desc = HashSet::<Id>::new();
                
                for child in node.children() {
                    Self::get_enode_descendents(egraph, *child, &mut result_desc, &mut visited);
                    result_desc.insert(*child);
                }

                enode_descendents.insert((node.clone(), class.id), result_desc);
            }
        }

        enode_descendents

    }

    fn get_enode_descendents (
        egraph: &'a EGraph<L, N>,
        eclass_id: Id,
        result_desc: &mut HashSet<Id>,
        visited: &mut HashSet<Id>
    ) {
        if visited.contains(&eclass_id) {
            return;
        }

        visited.insert(eclass_id);

        if let Some(class) = egraph.classes().find(|class| class.id == eclass_id) {
            for node in &class.nodes {
                for child in node.children() {
                    Self::get_enode_descendents(egraph, *child, result_desc, visited);
                    result_desc.insert(*child);
                }
            }
        }
        visited.remove(&eclass_id);
    }

    // fn find_descendents (
    //     egraph: &'a EGraph<L, N>,
    //     root: Id,
    // ) -> HashMap<Id, HashSet<Id>> {

    //     //let map_id_eclass: HashMap<Id, EClass<L, N>> = egraph.classes().map(|class| (class.id, class)).collect();

    //     let mut descendents: HashMap<Id, HashSet<Id>> = Default::default();
    //     let mut visited: HashSet<Id> = Default::default();

    //     Self::get_descendents(
    //         egraph,
    //         root,
    //         &mut descendents,
    //         &mut visited
    //     );

    //     descendents
    // }

    // fn get_descendents(
    //     egraph: &'a EGraph<L, N>,
    //     eclass_id: Id,
    //     descendents: &mut HashMap<Id, HashSet<Id>>,
    //     visited: &mut HashSet<Id>,  // Added visited set
    // ) {
    //     // Check if we already computed the descendants for this eclass_id
    //     if descendents.contains_key(&eclass_id) {
    //         return;
    //     }
    
    //     // Check if we've already visited this eclass_id to prevent cycles
    //     if visited.contains(&eclass_id) {
    //         eprintln!("Cycle detected at eclass_id: {:?}", eclass_id);
    //         return;
    //     }
    
    //     // Mark this eclass_id as visited
    //     visited.insert(eclass_id);
    
    //     // Find the eclass by its ID
    //     if let Some(class) = egraph.classes().find(|class| class.id == eclass_id) {
    //         let mut result_desc = HashSet::<Id>::new();
    
    //         // Iterate over the nodes (ENodes) in the class
    //         for node in &class.nodes {
    //             // Access each node's children
    //             eprintln!("node is {:?}", node);
    //             for child in node.children() {
    //                 // Recursively get the descendants of each child
    //                 Self::get_descendents(egraph, *child, descendents, visited);
    
    //                 // Get the child's descendants and union them with the current result set
    //                 if let Some(child_desc) = descendents.get(child) {
    //                     result_desc.extend(child_desc);
    //                 }
    //                 result_desc.insert(*child);
    //             }
    //         }
    
    //         // Store the result for this eclass_id in the descendents map
    //         descendents.insert(eclass_id, result_desc);
    //     } else {
    //         eprintln!("No class found for eclass_id: {:?}", eclass_id);
    //     }
    
    //     // After processing, remove from the visited set (this allows revisiting if needed for further unions)

    //     visited.remove(&eclass_id);
    // }
}