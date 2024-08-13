use egg::*;
use std::{
    cmp::Ordering,
    collections::{HashMap, HashSet},
};

use crate::cost;
use crate::{config::*, fhelang::FheLang};
pub struct Extractor<'a, CF: cost::CostFunction<FheLang>, N: Analysis<FheLang>> {
    cost_function: CF,
    costs: HashMap<Id, (f64, f64, f64, FheLang)>,
    egraph: &'a egg::EGraph<FheLang, N>,
}

impl<'a, CF, N> Extractor<'a, CF, N>
where
    CF: cost::CostFunction<FheLang>,
    N: Analysis<FheLang>,
{
    pub fn new(egraph: &'a EGraph<FheLang, N>, cost_function: CF) -> Self {
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
    pub fn find_best(&mut self, eclass: Id) -> (f64, RecExpr<FheLang>) {
        let mut expr = RecExpr::default();
        let (_, cost) = self.find_best_rec(&mut expr, eclass);
        (cost, expr)
    }

    fn find_best_rec(&mut self, expr: &mut RecExpr<FheLang>, eclass: Id) -> (Id, f64) {
        let id = self.egraph.find(eclass);
        let (best_cost, best_node) = match self
            .costs
            .get(&id)
            .map(|c| (ALPHA * c.0 + BETA * c.1 + GAMMA * c.2, c.3.clone()))
        {
            Some(result) => result.clone(),
            None => panic!("Failed to extract from eclass {}", id),
        };

        let node = best_node.map_children(|child| self.find_best_rec(expr, child).0);
        (expr.add(node), best_cost)
    }

    fn is_constant_multiplication(&self, node: &FheLang) -> bool {
        match node {
            FheLang::Mul([a, b]) => {
                let a = self.costs[&a].3.clone();
                let b = self.costs[&b].3.clone();
                if let FheLang::Constant(_) = a {
                    return true;
                }
                if let FheLang::Constant(_) = b {
                    return true;
                }
                return false;
            }
            _ => false,
        }
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
    /// - `Some(f64)`: The total cost of the node if all child costs are available.
    /// - `None`: If the cost of the node cannot be calculated due to missing child costs.
    fn node_total_cost(
        &mut self,
        node: &FheLang,
        map: &mut HashMap<Id, HashSet<Id>>,
    ) -> Option<(f64, f64, f64)> {
        let eg = &self.egraph;

        // Check if all children have their costs calculated
        let has_cost = |&id| self.costs.contains_key(&eg.find(id));
        if node.children().iter().all(has_cost) {
            let costs = &self.costs;

            // Get the cost of a child e-class
            let depth_f = |id| costs[&eg.find(id)].0.clone();
            let mul_depth_f = |id| costs[&eg.find(id)].1.clone();
            let op_costs_f = |id| costs[&eg.find(id)].2.clone();

            // Calculate the initial cost of the node using the cost function
            let depth = self.cost_function.depth(&node, depth_f);
            let mut mul_depth = self.cost_function.mul_depth(&node, mul_depth_f);
            let mut op_costs = self.cost_function.op_costs(&node, op_costs_f);

            // Handle nodes with one child
            if node.children().len() == 1 {
                return Some((depth, mul_depth, op_costs));
            }

            // Handle nodes with two children
            if node.children().len() == 2 {
                // Check if the operation is multiplication
                if let FheLang::Mul(..) = node {
                    // Get the first and second child nodes
                    if self.is_constant_multiplication(node) {
                        op_costs -= 99.0 * OP;
                        mul_depth -= 1.0;
                    }
                }
                let child_0 = node.children().iter().nth(0).unwrap();
                let child_1 = node.children().iter().nth(1).unwrap();

                // If both children are the same, subtract the cost of one child
                if child_0 == child_1 {
                    return Some((depth, mul_depth, op_costs - costs[&eg.find(*child_1)].2));
                }

                let sub_classes_class_0 = map.get(&child_0).unwrap();
                let sub_classes_class_1 = map.get(&child_1).unwrap();

                // If one child e-class belongs to the hierarchy of the other, subtract the cost of the contained e-class
                if sub_classes_class_0.contains(child_1) {
                    return Some((depth, mul_depth, op_costs - costs[&eg.find(*child_1)].2));
                }
                if sub_classes_class_1.contains(child_0) {
                    return Some((depth, mul_depth, op_costs - costs[&eg.find(*child_0)].2));
                }

                // Calculate the intersection of both hierarchies and subtract the cost of the shared operations
                let shared_sub_classes = sub_classes_class_0
                    .intersection(sub_classes_class_1)
                    .cloned()
                    .collect::<HashSet<Id>>();

                for id in shared_sub_classes {
                    let node = costs[&eg.find(id)].3.clone();

                    // Define costs for different operations

                    let op_cost: f64 = match node {
                        FheLang::Add(..) | FheLang::Minus(..) | FheLang::Neg(..) => OP,
                        FheLang::Rot(..) => OP * 50.0,
                        FheLang::Square(..) => OP * 80.0,
                        FheLang::Mul(..) => {
                            if self.is_constant_multiplication(&node) {
                                OP * 1.0
                            } else {
                                OP * 100.0
                            }
                        }
                        _ => LITERAL,
                    };

                    op_costs -= op_cost;
                }
                return Some((depth, mul_depth, op_costs));
            } else {
                // If the node has more than two children, return the calculated cost
                return Some((depth, mul_depth, op_costs));
            }
        }
        // Return None if the cost cannot be calculated
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

        // Iterate until no more changes are detected
        while did_something {
            did_something = false;

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
                        if ALPHA * new.0 + BETA * new.1 + GAMMA * new.2
                            != ALPHA * old.0 + BETA * old.1 + GAMMA * old.2
                        {
                            did_something = true;
                        }
                        self.costs.insert(class.id, new);
                    }
                    _ => (),
                }
            }
        }

        // Log an error message for any e-class that failed to compute a cost
        for class in self.egraph.classes() {
            if !self.costs.contains_key(&class.id) {
                eprintln!(
                    "Failed to compute cost for eclass {}: {:?}",
                    class.id, class.nodes
                )
            }
        }
    }

    fn cmp(a: &Option<(f64, f64, f64)>, b: &Option<(f64, f64, f64)>) -> Ordering {
        match (a, b) {
            (None, None) => Ordering::Equal,
            (None, Some(_)) => Ordering::Greater,
            (Some(_), None) => Ordering::Less,
            (Some(a), Some(b)) => {
                let x = ALPHA * a.0 + BETA * a.1 + GAMMA * a.2;
                let y = ALPHA * b.0 + BETA * b.1 + GAMMA * b.2;
                x.partial_cmp(&y).unwrap()
            }
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
    /// - `Some((f64, L))`: A tuple containing the minimum cost and the corresponding best e-node.
    /// - `None`: If no valid cost could be calculated for any e-node within the e-class.
    fn make_pass(
        &mut self,
        sub_classes: &mut HashMap<Id, HashSet<Id>>,
        eclass: &EClass<FheLang, N::Data>,
    ) -> Option<(f64, f64, f64, FheLang)> {
        let mut node_sub_classes: HashSet<Id> = HashSet::new();
        let nodes = eclass.nodes.clone();

        // Find the e-node with the minimum cost
        let (cost, node) = nodes
            .iter()
            .map(|n| (self.node_total_cost(n, sub_classes), n))
            .min_by(|a, b| Self::cmp(&a.0, &b.0))
            .unwrap_or_else(|| panic!("Can't extract, eclass is empty: {:#?}", eclass));

        match cost {
            // If no valid cost could be calculated, return None
            None => None,

            // If a valid cost is found
            Some(cost) => {
                // Update the hierarchy for the e-class based on the best e-node
                node.for_each(|id| {
                    node_sub_classes.insert(id);
                    node_sub_classes = node_sub_classes
                        .union(sub_classes.get(&id).unwrap())
                        .cloned()
                        .collect();
                });
                sub_classes.insert(eclass.id, node_sub_classes);

                Some((cost.0, cost.1, cost.2, node.clone()))
            }
        }
    }
}
