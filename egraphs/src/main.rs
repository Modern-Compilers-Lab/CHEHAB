#![allow(warnings)]
extern crate clap;
use clap::{App, Arg};
use egraphslib::*;
use std::time::Instant;
use crate::veclang::VecLang;
use egg::{RecExpr, Id, Language};
use rand::Rng;
use std::{env, fs};
/************************************/
use std::collections::BinaryHeap;
use std::cmp::Ordering; 
use std::cmp::Reverse; 
   
#[derive(Debug, Eq, PartialEq)]
struct State {
    cost: usize,
    max_vector_width : usize ,
    expression: RecExpr<VecLang>, // Additional data (e.g., node ID)
}

// Implement Ord for custom sorting based on cost
impl Ord for State {
    fn cmp(&self, other: &Self) -> Ordering {
        other.cost.cmp(&self.cost) // Min-heap: lower cost has higher priority
    }
}

// Implement PartialOrd to be consistent with Ord
impl PartialOrd for State {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

/************************************/
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
    .arg(
        Arg::with_name("rewrite_rule_family_index")
            .help("Specify initial rewrite_family_index")
            .required(true)
            .index(3),
    ) 
    .get_matches();
    /************* Parameters  **************/
    /****************************************/
    let timeout = 300 ; // in seconds 
    let node_limit = 100_000 ; 
    let mut rulesets_appplying_order  = vec![2,3,4];
    /****************************************/
    /**************Inputs********************/
    // Get a path string to parse a program.
    let path = matches.value_of("INPUT").unwrap();
    let vector_width: usize = matches
    .value_of("vector_width")
    .unwrap() 
    .parse()
    .expect("Number must be a valid usize");
    let rewrite_rule_family_index: usize = matches
    .value_of("rewrite_rule_family_index")
    .unwrap() 
    .parse()
    .expect("Number must be a valid usize");
    /*********************************************************/
    /************Load Input program Expression****************/
    let prog_str = fs::read_to_string(path).expect("Failed to read the input file.");
    let mut prog : RecExpr<VecLang>= prog_str.parse().unwrap();
    let mut input_prog : String = prog_str.parse().unwrap();
    eprintln!("****************************************************");
    //eprintln!("===> Input expression : {}",input_prog);
    eprintln!("****************************************************");
    /*********************************************************/
    // Record the start time
    let start_time = Instant::now();
    // Run rewriter
    eprintln!(
        "Running egg with timeout {:?}s, width: {:?} \n",
        timeout, vector_width
    );
    eprintln!("*********************Iterations******************************* \n");
    let mut current_cost = 0 ;
    let mut current_expr = prog.clone();
    let mut stop_reason = 0 ;
    /*********************************/
    let mut previous_cost = usize::MAX;
    let mut comp = 0;
    let mut noise_check_threhold_comp : usize =0;
    let mut iteration = rewrite_rule_family_index;
    let mut noise_check_threhold =  usize::MAX;
    let mut current_vector_width = vector_width ; 
    let mut stop_reached = 0 ;
    /** Extraction techniques 
        0 ==> GreedyExtractor
        1 ==> ExhaustiveExtractor
        2 ==> SimulatedAnnealingExtractor
    */
    let extracation_technic : usize = 0;
    while (comp != rulesets_appplying_order.len() && noise_check_threhold_comp!=noise_check_threhold){
        let (cost, best, stop_reason) = rules::run(&current_expr, timeout, current_vector_width,node_limit,rulesets_appplying_order[iteration%rulesets_appplying_order.len()],0);
        current_expr = best ; 
        current_cost = cost ;
        current_vector_width = rules::get_vector_width(&current_expr);
        if (current_cost == previous_cost){
            comp+=1;
        }else{
            previous_cost=current_cost ; 
            comp=1;
        }
        iteration = iteration + 1 ;
        noise_check_threhold_comp = noise_check_threhold_comp + 1;
        eprintln!("Best cost at iteration {}: {} ", iteration, current_cost);
    } 
    if (comp >= rulesets_appplying_order.len() ){
        stop_reached = 1;
    }
    let mut best_cost = current_cost ; 
    let mut best_expr = current_expr.clone(); 
    let duration = start_time.elapsed();
    /************************************************/
    eprintln!("****************************************************************");
    eprintln!("****************************************************************");
    eprintln!("===> Final_Cost: {} \n", best_cost);
    eprintln!("===> Vectorized Expression : {} \n", best_expr.to_string()); 
    eprintln!("****************************************************************");
    eprintln!("****************************************************************\n");
    /************************************************/
    // Print the results
    println!("{}", best_expr.to_string()); 
    println!("{} {} {} {}",current_vector_width,current_vector_width,stop_reached,iteration);
    //eprintln!("Time taken: {:?} to finish", duration);*/
}
