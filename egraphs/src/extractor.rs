use egg::*;
use std::{
    cmp::Ordering,
    collections::{HashMap, HashSet},
};
use std::time::Instant;
use std::vec::Vec;
use std::time::Duration;
use log::debug;
use crate::cost;
use crate::config::*;

pub struct Extractor<'a, CF: cost::CostFunction<L>, L: Language, N: Analysis<L>> {
    cost_function: CF,
    // the cost contains : depth, mul_depth, rotations, total operations
    costs: HashMap<Id, (f64, f64, f64, f64, L)>,
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
        sorting: bool,
        ) -> Self
    {
            let costs = HashMap::default();
            let mut extractor = Extractor {
                costs,
                egraph,
                cost_function,
            };

            let mut oredered_eclasses = Vec::new();
            if sorting {
                let start_sorting = Instant::now();
                oredered_eclasses = Self::semi_topological_sort(egraph, root);
                let end_sorting = start_sorting.elapsed();

                debug!("time to sort is : {:?}", end_sorting);
                extractor.find_costs(oredered_eclasses);
            } else {
                extractor.find_costs(oredered_eclasses);
            }

            extractor
    }

    /// Find the cheapest (lowest cost) represented `RecExpr` in the
    /// given eclass.
    pub fn find_best(&mut self, eclass: Id) -> (f64, RecExpr<L>) {
        let mut expr = RecExpr::default();
        let (_, cost) = self.find_best_rec(&mut expr, eclass);
        (cost, expr)
    }


    fn find_best_rec(&mut self, expr: &mut RecExpr<L>, eclass: Id) -> (Id, f64) {
        let id = self.egraph.find(eclass);
    
       
        let (best_cost, best_node) = match self
        .costs
        .get(&id)
        .map(|c| (ALPHA * c.0 + BETA * c.1 + GAMMA * c.2 + DELTA * c.3, c.4.clone())) {
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

    pub fn semi_topological_sort(egraph: &'a EGraph<L, N>, root: Id) -> Vec<(Id, usize)> {

        let mut order_map: HashMap<Id, usize> = HashMap::new();  // Store the order of each e-class
        let mut stack: Vec<(Id, usize)> = vec![(root, 0)];  // Stack to simulate DFS traversal (ID, current order)
        
        // Use this set to avoid revisiting eclasses
        let mut visited: HashSet<Id> = HashSet::new();
    
        while let Some((id, current_order)) = stack.pop() {
            // If this eclass has been visited before, skip it
            if visited.contains(&id) {
                continue;
            }
    
            visited.insert(id);  // Mark this eclass as visited
    
            // If this eclass has been visited before, update its order with the maximum of current and previous orders
            let existing_order = order_map.get(&id).copied();
            let new_order = existing_order.map_or(current_order, |prev_order| prev_order.max(current_order));
    
            // Assign the highest order to this eclass
            order_map.insert(id, new_order);
    
            let eclass = &egraph[id];  // Access the eclass once, instead of inside the loop
    
            // Traverse through all the enodes in this eclass
            for enode in eclass.iter() {
                // For each enode, traverse its children (which are eclasses)
                for &child_id in enode.children() {
                    // Push the child eclass to the stack with the incremented order
                    stack.push((child_id, new_order + 1));
                }
            }
        }
    
        // Collect the order_map into a vector and sort it by order in descending order
        let mut sorted_orders: Vec<(Id, usize)> = order_map.into_iter().collect();
        sorted_orders.sort_by(|a, b| b.1.cmp(&a.1));  // Sort by the order, high to low
    
        sorted_orders  // Return the sorted vector with eclass Ids and their assigned order
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
    /// - `Some(f64, f64, f64, f64)`: The total cost of the node if all child costs are available.
    /// - `None`: If the cost of the node cannot be calculated due to missing child costs.
    fn node_total_cost(
        &mut self, 
        node: &L, 
        map: &mut HashMap<Id, HashSet<Id>>
    ) -> Option<(f64, f64, f64, f64)> {
        let eg = &self.egraph;
    
        // Check if all children have their costs calculated
        let has_cost = |&id| self.costs.contains_key(&eg.find(id));

        if node.children().iter().all(has_cost) {
            let costs = &self.costs;

            let depth_f = |id| costs[&eg.find(id)].0.clone();
            let mul_depth_f = |id| costs[&eg.find(id)].1.clone();
            let rotations_f = |id| costs[&eg.find(id)].2.clone();
            let operations_cost_f = |id| costs[&eg.find(id)].3.clone();
            // let cost_f = |id| costs[&eg.find(id)].0.clone();

            let depth = self.cost_function.depth(&node, depth_f);
            let mul_depth = self.cost_function.mul_depth(&node, mul_depth_f);
            let rotations = self.cost_function.rotations(&node, rotations_f);
            let mut operations_cost = self.cost_function.operations_cost(&node, operations_cost_f);

            // let mut cost = self.cost_function.cost(&node, cost_f);
            let children = node.children();
    
            if children.len() == 1 {
                return Some((depth, mul_depth, rotations, operations_cost));
            }
    
            let mut shared_sub_classes: HashSet<Id> = HashSet::new();

            let start = Instant::now();
    
            // Compare all pairs of children
            for (i, &id_i) in children.iter().enumerate() {

                let sub_classes_i = map.get(&id_i).unwrap();
                
                for &id_j in children.iter().skip(i + 1) {
    
                    // If both children are the same, subtract the cost of one child
                    if id_i == id_j {
                        return Some((depth, mul_depth, rotations, operations_cost - costs[&eg.find(id_i)].3));
                    }
    
                    let sub_classes_j = map.get(&id_j).unwrap();
    
                    // If one child belongs to the hierarchy of the other, subtract the contained class cost
                    if sub_classes_i.contains(&id_j) {
                        return Some((depth, mul_depth, rotations, operations_cost - costs[&eg.find(id_i)].3));
                    }
                    if sub_classes_j.contains(&id_i) {
                        return Some((depth, mul_depth, rotations, operations_cost - costs[&eg.find(id_i)].3));
                    }
    
                    // Calculate the intersection of both hierarchies and subtract the cost of shared operations
                    let shared = sub_classes_i
                        .intersection(sub_classes_j)
                        .cloned()
                        .collect::<HashSet<Id>>();
    
                    shared_sub_classes = shared_sub_classes.union(&shared).cloned().collect();
                }
            }

            let end = start.elapsed();
            debug!("the time for looping is : {:?}", end);
    
            // Adjust the cost based on shared sub-classes
            for id in shared_sub_classes {
                let node = costs[&eg.find(id)].4.clone();
                let op = node.to_string();
    
                // Define operation costs
                const LITERAL: f64 = 0.0;
                const STRUCTURE: f64 = 2000.0;
                const VEC_OP: f64 = 1.0;
                const OP: f64 = 1.0;
    
                let shared_op_costs: f64 = match op.as_str() {
                    "+" | "*" | "-" | "neg" => OP * 10_000.0,
                    "<<" => VEC_OP * 50.0,
                    "Vec" => STRUCTURE,
                    "VecAdd" | "VecMinus" | "VecNeg" => VEC_OP,
                    "VecMul" => VEC_OP * 100.0,
                    "VecAddRotF" | "VecMinusRotF" | "VecAddRotS" | "VecMinusRotS"=> VEC_OP * 1051.0,
                    "VecMulRotF" | "VecMulRotS" => VEC_OP * 2150.0,
                    "VecAddRotP" | "VecMinusRotP" => VEC_OP * 5_000.0,
                    "VecMulRotP" => VEC_OP * 7_000.0,
                    _ => LITERAL,
                };
    
                operations_cost -= shared_op_costs;
            }
    
            return Some((depth, mul_depth, rotations, operations_cost));
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
    fn find_costs(&mut self, oredered_eclasses : Vec<(Id, usize)>) {

        let mut did_something = true;
        let mut sub_classes: HashMap<Id, HashSet<Id>> = HashMap::new();
        let mut i = 0;
        let mut eclasses_iter: Vec<&EClass<L, N::Data>> = Vec::new();

        // debug!("ordered eclasses : {:?}", oredered_eclasses);

        if oredered_eclasses.is_empty() {
            for ecl in self.egraph.classes() {
                eclasses_iter.push(ecl); // Add the e-class to the eclasses_iter list
            }
        } else {
            for ecl in oredered_eclasses {
                let eclass = &self.egraph[ecl.0]; // Access the e-class using the ordered Ids
                eclasses_iter.push(eclass); // Add the e-class to the eclasses_iter list
            }
        }


        // Iterate until no more changes are detected
        while did_something {
            // Start timer for this iteration
            let start_time = Instant::now();
            let time_to_pdate = Duration::new(0, 0);

            did_something = false;
            i += 1;
            debug!("iteration number {:?}", i);

            for class in &eclasses_iter {
                let pass = self.make_pass(&mut sub_classes, class);
                match (self.costs.get(&class.id), pass) {
                    // If the cost is calculated for the first time
                    (None, Some(new)) => {
                        self.costs.insert(class.id, new);
                        did_something = true;
                    }
                    // If the cost is already calculated and there is a change
                    (Some(old), Some(new)) => {
                        if ALPHA * new.0 + BETA * new.1 + GAMMA * new.2 + DELTA * new.3
                        != ALPHA * new.0 + BETA * old.1 + GAMMA * old.2 + DELTA * old.3 {
                            // self.costs.insert(class.id, new);
                            did_something = true;
                        }
                        self.costs.insert(class.id, new);
                    }

                    _ => (),
                }

            }

            // Measure the time for the current iteration
            let duration = start_time.elapsed();
            debug!("Iteration {} took {:?}", i, duration);
            debug!("did_something is {:?}", did_something);
            debug!("total_time to update is {:?} for iteration {:}", time_to_pdate.as_secs_f64(), i);
        }

        debug!("Total number of iterations: {}", i);

        // Log an error message for any e-class that failed to compute a cost
        for class in self.egraph.classes() {
            if !self.costs.contains_key(&class.id) {
                debug!(
                    "Failed to compute cost for eclass {}: {:?}",
                    class.id, class.nodes
                );
            }
        }
    }

    fn cmp(a: &Option<(f64, f64, f64, f64)>, b: &Option<(f64, f64, f64, f64)>) -> Ordering {
        match (a, b) {
            (None, None) => Ordering::Equal,
            (None, Some(_)) => Ordering::Greater,
            (Some(_), None) => Ordering::Less,
            (Some(a), Some(b)) => {
                let x = ALPHA * a.0 + BETA * a.1 + GAMMA * a.2 + DELTA * a.3;
                let y = ALPHA * b.0 + BETA * b.1 + GAMMA * b.2 + DELTA * b.3;
                x.partial_cmp(&y).unwrap()
            },
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
    /// - `Some((f64, f64, f64, f64, L))`: A tuple containing the minimum cost and the corresponding best e-node.
    /// - `None`: If no valid cost could be calculated for any e-node within the e-class.

    fn make_pass(
        &mut self,
        sub_classes: &mut HashMap<Id, HashSet<Id>>,
        eclass: &EClass<L, N::Data>,
        // enode_descendents: HashMap<(L, Id), HashSet<Id>>,
    ) -> Option<(f64, f64, f64, f64, L)> {
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
        debug!("Cost calculation took: {:?}", cost_calculation_duration);

        match cost {
            // If no valid cost could be calculated, return None
            None => {
                return None;
            },

            // If a valid cost is found
            Some(cost) => {
                debug!("new cost {:?} found , the enodes {:?} desc are updated", cost, node);

                node.for_each(|id| {
                    node_sub_classes.insert(id);

                    if let Some(sub_class_set) = sub_classes.get(&id) {
                        node_sub_classes.extend(sub_class_set.iter());
                    }
                });

                sub_classes.insert(eclass.id, node_sub_classes);

               

                // Print the total time taken for the function
                debug!("Total time for make_pass: {:?}", start_time.elapsed());

                Some((cost.0, cost.1, cost.2, cost.3, node.clone()))
            }
        }
    }

    // fn find_enode_descendents(
    //     egraph: &'a EGraph<L, N>,
    // ) -> HashMap<(L, Id), HashSet<Id>>
    // {
    //     let mut enode_descendents: HashMap<(L, Id), HashSet<Id>> = HashMap::new();
    //     let mut visited: HashSet<Id> = HashSet::new();

    //     for class in egraph.classes() {
    //         for node in &class.nodes {
    //             let mut result_desc = HashSet::<Id>::new();
                
    //             for child in node.children() {
    //                 Self::get_enode_descendents(egraph, *child, &mut result_desc, &mut visited);
    //                 result_desc.insert(*child);
    //             }

    //             enode_descendents.insert((node.clone(), class.id), result_desc);
    //         }
    //     }

    //     enode_descendents

    // }

    // fn get_enode_descendents (
    //     egraph: &'a EGraph<L, N>,
    //     eclass_id: Id,
    //     result_desc: &mut HashSet<Id>,
    //     visited: &mut HashSet<Id>
    // ) {
    //     if visited.contains(&eclass_id) {
    //         return;
    //     }

    //     visited.insert(eclass_id);

    //     if let Some(class) = egraph.classes().find(|class| class.id == eclass_id) {
    //         for node in &class.nodes {
    //             for child in node.children() {
    //                 Self::get_enode_descendents(egraph, *child, result_desc, visited);
    //                 result_desc.insert(*child);
    //             }
    //         }
    //     }
    //     visited.remove(&eclass_id);
    // }
}
