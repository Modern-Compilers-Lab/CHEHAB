use egg::*;
use std::{
    cmp::Ordering,
    collections::{HashMap, HashSet},
};

use crate::cost;
pub struct Extractor<'a, CF: cost::CostFunction<L>, L: Language, N: Analysis<L>> {
    cost_function: CF,
    costs: HashMap<Id, (f64, L)>,
    egraph: &'a egg::EGraph<L, N>,
}

impl<'a, CF, L, N> Extractor<'a, CF, L, N>
where
    CF: cost::CostFunction<L>,
    L: Language,
    N: Analysis<L>,
{
    pub fn new(egraph: &'a EGraph<L, N>, cost_function: CF) -> Self {
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
    pub fn find_best(&mut self, eclass: Id) -> (f64, RecExpr<L>) {
        let mut expr = RecExpr::default();
        let (_, cost) = self.find_best_rec(&mut expr, eclass);
        (cost, expr)
    }

    fn find_best_rec(&mut self, expr: &mut RecExpr<L>, eclass: Id) -> (Id, f64) {
        let id = self.egraph.find(eclass);
        let (best_cost, best_node) = match self.costs.get(&id) {
            Some(result) => result.clone(),
            None => panic!("Failed to extract from eclass {}", id),
        };

        let node = best_node.map_children(|child| self.find_best_rec(expr, child).0);
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
    /// - `Some(f64)`: The total cost of the node if all child costs are available.
    /// - `None`: If the cost of the node cannot be calculated due to missing child costs.
    fn node_total_cost(&mut self, node: &L, map: &mut HashMap<Id, HashSet<Id>>) -> Option<f64> {
        let eg = &self.egraph;

        // Check if all children have their costs calculated
        let has_cost = |&id| self.costs.contains_key(&eg.find(id));
        if node.children().iter().all(has_cost) {
            let costs = &self.costs;

            // Get the cost of a child e-class
            let cost_f = |id| costs[&eg.find(id)].0.clone();

            // Calculate the initial cost of the node using the cost function
            let mut cost = self.cost_function.cost(&node, cost_f);

            // Handle nodes with one child
            if node.children().len() == 1 {
                return Some(cost);
            }

            // Handle nodes with two children
            if node.children().len() == 2 {
                let id_0 = node.children().iter().nth(0).unwrap();
                let id_1 = node.children().iter().nth(1).unwrap();

                // If both children are the same, subtract the cost of one child
                if id_0 == id_1 {
                    return Some(cost - costs[&eg.find(*id_1)].0);
                }

                let sub_classes_class_0 = map.get(&id_0).unwrap();
                let sub_classes_class_1 = map.get(&id_1).unwrap();

                // If one child e-class belongs to the hierarchy of the other, subtract the cost of the contained e-class
                if sub_classes_class_0.contains(id_1) {
                    return Some(cost - costs[&eg.find(*id_1)].0);
                }
                if sub_classes_class_1.contains(id_0) {
                    return Some(cost - costs[&eg.find(*id_0)].0);
                }

                // Calculate the intersection of both hierarchies and subtract the cost of the shared operations
                let shared_sub_classes = sub_classes_class_0
                    .intersection(sub_classes_class_1)
                    .cloned()
                    .collect::<HashSet<Id>>();

                for id in shared_sub_classes {
                    let node = costs[&eg.find(id)].1.clone();

                    // Define costs for different operations
                    const LITERAL: f64 = 0.001;
                    const STRUCTURE: f64 = 2000.0;
                    const VEC_OP: f64 = 1.0;
                    const OP: f64 = 1.0;

                    let op = node.display_op().to_string();
                    let op_costs: f64 = match op.as_str() {
                        "+" | "*" | "-" | "neg" => OP,
                        "<<" => VEC_OP * 10.0,
                        "Vec" => STRUCTURE,
                        "VecAdd" | "VecMinus" => VEC_OP,
                        "VecMul" => VEC_OP * 100.0,
                        _ => LITERAL,
                    };

                    cost -= op_costs;
                }
                return Some(cost);
            } else {
                // If the node has more than two children, return the calculated cost
                return Some(cost);
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
                        if new.0 != old.0 {
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

    fn cmp(a: &Option<f64>, b: &Option<f64>) -> Ordering {
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
    /// - `Some((f64, L))`: A tuple containing the minimum cost and the corresponding best e-node.
    /// - `None`: If no valid cost could be calculated for any e-node within the e-class.
    fn make_pass(
        &mut self,
        sub_classes: &mut HashMap<Id, HashSet<Id>>,
        eclass: &EClass<L, N::Data>,
    ) -> Option<(f64, L)> {
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

                Some((cost, node.clone()))
            }
        }
    }
}
