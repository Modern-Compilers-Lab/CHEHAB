extern crate clap;
use clap::{App, Arg};
use egraphslib::*;
use std::time::Instant;
use crate::veclang::VecLang;
use egg::{RecExpr, Id, Language};
use rand::Rng;
/*************************************************************************/
fn ast_depth(expr: &RecExpr<VecLang>) -> usize {
    fn depth_helper(id: Id, expr: &RecExpr<VecLang>) -> usize {
        let node = &expr[id];
        if node.children().is_empty() {
            0 // Leaf node, depth is 1
        } else {
            // Depth is 1 + max depth of all child nodes
            1 + node.children().iter().map(|&child| depth_helper(child, expr)).max().unwrap_or(0)
        }
    }
    // Start with the root node (usually the last node in `RecExpr`)
    depth_helper(Id::from(expr.as_ref().len() - 1), expr)
}
/*****************************************************************************/
fn main() {
    let matches = App::new("Rewriter")
        .arg(
            Arg::with_name("INPUT")
                .help("Sets the input file")
                .required(true)
                .index(1),
        )
        .arg(
            Arg::with_name("vector_width")
                .help("Sets the vector_width")
                .required(true)
                .index(2),
        ) 
        .get_matches();

    use std::{env, fs};

    // Get a path string to parse a program.
    let path = matches.value_of("INPUT").unwrap();
    let timeout = env::var("TIMEOUT")
        .ok()
        .and_then(|t| t.parse::<u64>().ok())
        .unwrap_or(300);
    let prog_str = fs::read_to_string(path).expect("Failed to read the input file.");
    let mut prog : RecExpr<VecLang>= prog_str.parse().unwrap();
    let vector_width: usize = matches
        .value_of("vector_width")
        .unwrap()
        .parse()
        .expect("Number must be a valid usize");

    // Record the start time
    let start_time = Instant::now();

    // Run rewriter
    eprintln!(
        "Running egg with timeout {:?}s, width: {:?}",
        timeout, vector_width
    );
    
    /*let mut best_cost = 0;
    let mut best_expr = prog.clone();
    let mut best_cost1 = 0;
    let mut best_expr1 = prog.clone();
    let mut stop_reason = 0 ;
    let mut rulesets_appplying_order  = vec![0,1];
    let mut node_limit = 100_000 ;*/
    /*
    for iteration in 0..10 {
        //println!("Iteration: {}", iteration + 1);
        // Run the rule optimization
        let (cost, best) = rules::run(&best_expr, timeout, vector_width,rulesets_appplying_order[iteration%rulesets_appplying_order.len()],0);
        // Update `prog` with the best expression from the current iteration
        best_expr = best ;
        // Store the best cost and expression
        best_cost = cost;
        eprintln!("Best cost at iteration {}: {} \n\n", iteration + 1, best_cost);
        //println!("Best expression at iteration {}: {}", iteration + 1, best_expr);
    }
    // vec![3]
    // iteration in 0..6
    rulesets_appplying_order  = vec![3,2];
    for iteration in 0..10 {
        let (cost, best) = rules::run(&best_expr, timeout, vector_width,rulesets_appplying_order[iteration%rulesets_appplying_order.len()],0);
        best_expr = best ;
        best_cost = cost;
        eprintln!("Best cost at iteration {}: {} \n\n", iteration + 1, best_cost);
    }
    eprintln!("\n\n******************* Continue working with exhaustive extraction *************************\n\n");
    rulesets_appplying_order  = vec![1];
    for iteration in 0..1 {
        let (cost, best) = rules::run(&best_expr, timeout, vector_width,rulesets_appplying_order[iteration%rulesets_appplying_order.len()],0);
        best_expr = best ;
        best_cost = cost;
        eprintln!("Best cost at iteration {}: {} \n\n", iteration + 1, best_cost);
    }*/
    /*for iteration in 0..10 {
        //println!("Iteration: {}", iteration + 1);
        // Run the rule optimization
        let (cost, best) = rules::run(&best_expr, timeout, vector_width,rulesets_appplying_order[iteration%rulesets_appplying_order.len()],0);
        // Update `prog` with the best expression from the current iteration
        best_expr = best ;
        // Store the best cost and expression
        best_cost = cost;
        eprintln!("Best cost at iteration {}: {} \n\n", iteration + 1, best_cost);
        //println!("Best expression at iteration {}: {}", iteration + 1, best_expr);
    }
    // vec![3]
    // iteration in 0..6
    rulesets_appplying_order  = vec![2,3];
    for iteration in 0..8 {
        let (cost, best) = rules::run(&best_expr, timeout, vector_width,rulesets_appplying_order[iteration%rulesets_appplying_order.len()],0);
        best_expr = best ;
        best_cost = cost;
        eprintln!("Best cost at iteration {}: {} \n\n", iteration + 1, best_cost);
    }
    eprintln!("\n\n******************* Continue working with exhaustive extraction *************************\n\n");
    rulesets_appplying_order  = vec![1];
    for iteration in 0..1 {
        let (cost, best) = rules::run(&best_expr, timeout, vector_width,rulesets_appplying_order[iteration%rulesets_appplying_order.len()],2);
        best_expr = best ;
        best_cost = cost;
        eprintln!("Best cost at iteration {}: {} \n\n", iteration + 1, best_cost);
    }*/
    /*******************************************************************************************************
    let mut rulesets_appplying_order  = vec![0,1];
    let mut iteration = 0;
    while iteration < 10 {
        //println!("Iteration: {}", iteration + 1);
        // Run the rule optimization
        let (cost, best, stop_reason) = rules::run(&best_expr, timeout, vector_width,node_limit,rulesets_appplying_order[iteration%rulesets_appplying_order.len()],0);
        if (stop_reason==1)&&(iteration%rulesets_appplying_order.len()==0) {
            if best_cost1 == 0 {
               best_expr1 = best.clone() ;
               best_cost1 = cost ;
               best_expr = prog.clone() ;
               best_cost = cost;
               node_limit = (node_limit/2).max(25000); 
               eprintln!("Best cost at iteration {}: {} , stop_reason : {} \n\n", iteration + 1, best_cost,stop_reason);
               eprintln!("\n====================>reapplying egrahs with node limit {} \n",node_limit);
               iteration = 0 ;
            }else{
                if cost < best_cost1 {
                    best_expr1 = best.clone() ;
                    best_cost1 = cost ;
                    best_expr = prog.clone();
                    best_cost = cost;
                    node_limit = (node_limit/2).max(25000);
                    eprintln!("Best cost at iteration {}: {} , stop_reason : {} \n\n", iteration + 1, best_cost,stop_reason);
                    eprintln!("\n====================>reapplying egrahs with node limit {} \n",node_limit);
                    iteration = 0 ; 
                }else{
                    best_cost = best_cost1 ;
                    best_expr = best_expr1 ;
                    eprintln!("\n====>End , cost hasnt improved, current iteration {} , previous cost : {} , current_cost : {} \n\n", iteration , best_cost1 ,cost);
                    break ;
                } // 244006
            }
        }else{
            best_expr = best ;
            best_cost = cost;
            eprintln!("Best cost at iteration {}: {} , stop_reason : {} \n\n", iteration, best_cost,stop_reason);
            iteration=iteration+1;
        }
        // 446002 , 244006 , 40019 
        // 446002 , 244006 , 44021
    }
    rulesets_appplying_order  = vec![1];
    for iteration in 0..5 {
        let (cost, best, stop_reason) = rules::run(&best_expr, timeout, vector_width,node_limit,rulesets_appplying_order[iteration%rulesets_appplying_order.len()],0);
        best_expr = best ;
        best_cost = cost;
        eprintln!("Best cost at iteration {}: {} \n\n", iteration + 1, best_cost);
    }*/

    /*******************************************************************************************************************/
    /*******************************************************************************************************************/
    /*******************************************************************************************************************/
    let mut current_cost = 0 ;
    let mut current_expr = prog.clone();
    let mut stop_reason = 0 ;
    let mut rulesets_appplying_order  = vec![0,1];
    let mut node_limit = 100_000 ;
    let mut final_node_limit = node_limit ;
    /*********************************/
    /*********************************/
    let mut temperature : f64 = 100.0 ;
    let cooling_rate : f64 = 0.99 ;
    let mut rng = rand::thread_rng();
    let mut iteration = 0 ;
    /*********************Generate initial solution****************************************
    let mut stop = false ;
    while (!stop){
        let (new_cost, new_expr, stop_reason) = rules::run(&current_expr, timeout, vector_width,node_limit,rulesets_appplying_order[iteration%rulesets_appplying_order.len()],0);
        current_expr = new_expr ;
        current_cost = new_cost ;
        if (stop_reason == 1) && (iteration%rulesets_appplying_order.len() == 0){
            stop = true ;
        }
        iteration = iteration + 1 ;
    }
    eprintln!("==> Initial best solution generated after {} Iterations with Cost {}: \n\n", iteration, current_cost);
    let mut best_expr = current_expr.clone();
    let mut best_cost = current_cost ;
    let mut current_sat_expr = current_expr.clone();
    let mut current_sat_cost = current_cost ;
    /***********************************************************************************************/
    let mut max_iter = (iteration + 1) * 2 + 1  ;
    iteration = 0 ;
    node_limit = (node_limit/2).max(25000);
    current_expr = prog.clone();
    while iteration < max_iter {
        let (new_cost, new_expr, stop_reason) = rules::run(&current_expr, timeout, vector_width,node_limit,rulesets_appplying_order[iteration%rulesets_appplying_order.len()],0);
        if (stop_reason==1)&&(iteration%rulesets_appplying_order.len()==0){
            let cost_diff: f64 = new_cost as f64 - current_sat_cost as f64;
            let acceptance_prob = if cost_diff > 0.0 {
                (-cost_diff / temperature).exp()
            } else {
                1.0
            };
            let prob_rng = rng.gen::<f64>() ;
            eprintln!("prob_rng : {} , acceptance_prob : {} , new_cost : {} , best_cost : {} ,current_sat_cost : {}",prob_rng,acceptance_prob,new_cost,best_cost,current_sat_cost);
            if prob_rng < acceptance_prob {
                current_sat_expr = new_expr;
                current_sat_cost = new_cost;
                if new_cost < best_cost {
                    eprintln!("****************Best expression updated **********************");
                    best_expr = current_sat_expr.clone();
                    best_cost = current_sat_cost;
                    final_node_limit = node_limit ;
                }
            }
            node_limit = (node_limit/2).max(25000);
            eprintln!("\n====================>reapplying egrahs with node limit {} \n",node_limit);
            current_expr = prog.clone();
            current_cost = new_cost ;
        }else{
            current_expr = new_expr ;
            current_cost = new_cost ;
        }
        eprintln!("Cost at iteration {}: {} , stop_reason : {} , node_limit : {} \n\n", iteration, current_cost,stop_reason,node_limit);
        temperature *= cooling_rate;
        iteration=iteration+1;
    }
    *****************************************************************************************************************/
    /*
        0 => {rules = rules0(vector_width);},
        1 => {rules = vector_rules(vector_width);},
        2 => {rules = addition_rules(vector_width);},
        3 => {rules = min_rules(vector_width);},
        4 => {rules = multiplication_rules(vector_width);},
        5 => {rules = vector_assoc_add_rules(vector_width);},
        6 => {rules = vector_assoc_min_rules(vector_width);},
        7 => {rules = vector_assoc_mul_rules(vector_width);},
        8 => {rules = vector_assoc_add_mul_rules(vector_width);},
        9 => {rules = vector_assoc_add_min_rules(vector_width);},
        10 => {rules = vector_assoc_min_mul_rules(vector_width);},
    */
    let mut best_cost = current_cost ;
    let mut best_expr = current_expr.clone();
    rulesets_appplying_order  = vec![2,4];
    let mut stop = false ;
    while (!stop){
        let (cost, best, stop_reason) = rules::run(&best_expr, timeout, vector_width,node_limit,rulesets_appplying_order[iteration%rulesets_appplying_order.len()],0);
        best_expr = best ;
        best_cost = cost ;
        if (stop_reason == 1){
            stop = true ;
        }
        iteration = iteration + 1 ;
    }
    eprintln!("\n\n******************* Continue working with Simulated annealing *************************\n\n");
    rulesets_appplying_order  = vec![5,8];
    let mut best_depth = ast_depth(&best_expr);
    current_expr = best_expr.clone();
    current_cost = best_cost ;
    for iteration in 0..6 {
        let (cost, best , stop_reason) = rules::run(&current_expr, timeout, vector_width, final_node_limit,rulesets_appplying_order[iteration%rulesets_appplying_order.len()],0);
        current_expr = best ;
        current_cost = cost;
        let depth = ast_depth(&current_expr);
        if depth < best_depth {
            best_depth=depth ; 
            best_expr = current_expr.clone() ;
        }
        eprintln!("Best cost at iteration {}: {} , Depth {} \n\n", iteration + 1, current_cost,depth);
    }
    /**********************************************************************************************************/
    /**********************************************************************************************************
    let mut rulesets_appplying_order  = vec![0,1];
    let mut best_cost = 0;
    let mut best_expr = prog;
    let node_limit = 100_000 ;
    for iteration in 0..10 {
        //println!("Iteration: {}", iteration + 1);
        // Run the rule optimization
        let (cost, best , stop_reason) = rules::run(&best_expr, timeout, vector_width,node_limit,rulesets_appplying_order[iteration%rulesets_appplying_order.len()],0);
        // Update `prog` with the best expression from the current iteration
        best_expr = best ;
        // Store the best cost and expression
        best_cost = cost;
        eprintln!("Best cost at iteration {}: {} \n\n", iteration + 1, best_cost);
        //println!("Best expression at iteration {}: {}", iteration + 1, best_expr);
    }
    // vec![3]
    // iteration in 0..6
    rulesets_appplying_order  = vec![2,3];
    for iteration in 0..8 {
        let (cost, best, stop_reason) = rules::run(&best_expr, timeout, vector_width,node_limit,rulesets_appplying_order[iteration%rulesets_appplying_order.len()],0);
        best_expr = best ;
        best_cost = cost;
        eprintln!("Best cost at iteration {}: {} \n\n", iteration + 1, best_cost);
    }
    eprintln!("\n\n******************* Continue working with exhaustive extraction *************************\n\n");
    rulesets_appplying_order  = vec![1];
    for iteration in 0..1 {
        let (cost, best, stop_reason) = rules::run(&best_expr, timeout, vector_width,node_limit, rulesets_appplying_order[iteration%rulesets_appplying_order.len()],2);
        best_expr = best ;
        best_cost = cost;
        eprintln!("Best cost at iteration {}: {} \n\n", iteration + 1, best_cost);
    }*/
    let duration = start_time.elapsed();
    // Print the results
    println!("{}", best_expr.to_string()); /* Pretty print with width 80 */
    eprintln!("\nCost: {}", best_cost);
    //eprintln!("Time taken: {:?} to finish", duration);*/
}
