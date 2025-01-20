extern crate clap;
use clap::{App, Arg};
use egraphslib::*;
use std::time::Instant;
use crate::veclang::VecLang;
use egg::{RecExpr, Id, Language};
use rand::Rng;
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
    eprintln!("the input expression is : {:?}", prog_str);

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
    
   
    
    /*******************************************************************************************************************/
    /*******************************************************************************************************************/
    let mut current_cost = 0 ;
    let mut current_expr = prog.clone();
    let mut stop_reason = 0 ;
    let mut rulesets_appplying_order  = vec![0,1];
    let mut node_limit = 100 ;
    /*********************************/
    /*********************************/
    let mut temperature : f64 = 100.0 ;
    let cooling_rate : f64 = 0.99 ;
    let mut rng = rand::thread_rng();
    let mut iteration = 0 ;
   
    rulesets_appplying_order  = vec![2,3,4,5];
    let mut previous_cost = usize::MAX;
    node_limit = 100_000 ;
    let mut comp = 0;
    let mut current_vector_width = vector_width ; 
    while comp != rulesets_appplying_order.len(){
        let (cost, best) = rules::run(&current_expr, timeout, current_vector_width,node_limit,rulesets_appplying_order[iteration%rulesets_appplying_order.len()],0);
        current_expr = best ; 
        current_cost = cost ;
        current_vector_width = rules::get_vector_width(&current_expr);
        eprintln!("===> vector width in this iteration: {}",current_vector_width);
        if current_cost == previous_cost{
            comp+=1;
        }else{
            previous_cost=current_cost ;
            comp=0;
        }
        iteration = iteration + 1 ;
        eprintln!("Best cost at iteration {}: {} ", iteration + 1, current_cost);
        //eprintln!("Obtained expression ==> : {}", current_expr.to_string());
    }
    /*****************************************************************************/
    let mut best_cost = current_cost ;
    let mut best_expr = current_expr.clone(); 
    rulesets_appplying_order  = vec![9,10,11,12,13,14,15];
    let mut best_depth = rules::ast_depth(&best_expr);
    // for iteration in 0..28 {
    //     let (cost, best , stop_reason) = rules::run(&current_expr, timeout, vector_width, node_limit,rulesets_appplying_order[iteration%rulesets_appplying_order.len()],0);
    //     let depth = rules::ast_depth(&best);
    //     if depth < best_depth {
    //         best_depth=depth ; 
    //         current_expr = best ; 
    //         best_expr = current_expr.clone() ;
    //     }
    //     eprintln!("Best cost at iteration {}: {} , Depth {} \n\n", iteration + 1, cost,depth);
    // }
    let duration = start_time.elapsed();
    // Print the results
    println!("{}", best_expr.to_string()); /* Pretty print with width 80 */
    eprintln!("best expression : {:?}", best_expr.to_string());
    println!("{} {}",current_vector_width,current_vector_width);
    eprintln!("\n===> Final expression depth : {}", rules::ast_depth(&best_expr));
    eprintln!("\nCost: {}", best_cost);
    //eprintln!("Time taken: {:?} to finish", duration);*/
}
