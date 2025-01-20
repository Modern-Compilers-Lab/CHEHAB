extern crate clap;
use clap::{App, Arg};
use egraphslib::*;
use std::time::Instant;
use crate::rules_2;
use log::debug;
use egg::*;
use crate::veclang::VecLang;
use crate::config::*;

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
            Arg::with_name("benchmark_type")
            .help("Specify the type of the benchamark to select the rules to apply")
            .required(true)
            .index(3)
        )
        .get_matches();

    use std::{env, fs};

    let vector_width: usize = matches
        .value_of("vector_width")
        .unwrap()
        .parse()
        .expect("Number must be a valid usize");

    let benchmark_type: usize = matches
        .value_of("benchmark_type")
        .unwrap()
        .parse()
        .expect("Number must be a valid usize");

    // Get a path string to parse a program.
    let path = matches.value_of("INPUT").unwrap();
    let timeout = env::var("TIMEOUT")
        .ok()
        .and_then(|t| t.parse::<u64>().ok())
        .unwrap_or(300);
    let prog_str = fs::read_to_string(path).expect("Failed to read the input file.");
    eprintln!("the input expression is : {:?}", prog_str);

     // if the benchmark is unstrucutred (10), we remove the initial part 'Vec(' which is added for
    // syntactic considerations, else, we let it 

      // Some parameters
      let rule_filtering = false;
      let sorting = false;
      let exp_rules = false;


    if benchmark_type == UNSTRUCTURED_WITH_ONE_OUTPUT {   // one output , unstructured
        let mut prog_str = prog_str.trim().to_string(); // Trim any leading/trailing whitespace
        debug!("remowing useless chars");
        // Remove "(Vec" at the start
        prog_str = prog_str.strip_prefix("(Vec ").unwrap_or(&prog_str).to_string(); // Remove the prefix just for rules considerations
        // Remove the last character (if it exists)
        prog_str.pop();
        prog_str.pop();

            // Print the cleaned-up expression
        debug!("The cleaned expression is: {:?}", prog_str);
        let prog = prog_str.parse().unwrap();

        // Record the start time
        let start_time = Instant::now();
        // Run rewriter
        eprintln!(
            "Running egg with benchmark_type {:?}, timeout {:?}s, width: {:?}, rule_filtering: {:?}, sorting: {:?}, exp-rules: {:?}",
            benchmark_type, timeout, vector_width, rule_filtering, sorting, exp_rules
        );
        let (cost, best) = 
            rules::run(&prog, timeout, benchmark_type, vector_width, rule_filtering, sorting, exp_rules, 0 /*rules set order is not required here*/);

        // Record the end time
        let duration = start_time.elapsed();

        // Print the results

        println!("{}", best.to_string()); /* Pretty print with width 80 */
        /*the value of the final vector width is not the follwed , this value is written in the file to facilitate the implementation
        and the real value will be calculated in the file compiler.cpp to generate the correct files after
        writing this value is important to avoid dealing with each case separaltelyn
        */
        println!("{} {}",vector_width,vector_width);
        eprintln!("\nCost: {}", cost);
        eprintln!("Time taken: {:?} to finish", duration);
        
    } else {
        let mut current_cost = 0.0 ;
      
        let mut iteration = 0 ;
        let rulesets_appplying_order  = vec![2,3,4,5];
        let mut previous_cost = f64::MAX;
        let mut comp = 0;
        let mut current_expr : RecExpr<VecLang>= prog_str.parse().unwrap();


        let mut current_vector_width = vector_width; 
        while comp != rulesets_appplying_order.len() {
            let (cost, best) = rules::run(&current_expr, timeout, benchmark_type, current_vector_width,rule_filtering, sorting, exp_rules, rulesets_appplying_order[iteration%rulesets_appplying_order.len()]);
            current_expr = best ; 
            current_cost = cost ;
            current_vector_width = rules_2::get_vector_width(&current_expr);
            eprintln!("===> vector width in this iteration : {:?}", current_vector_width);
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
        let best_cost = current_cost ;
        let best_expr = current_expr.clone(); 
        // Print the results
        println!("{}", best_expr.to_string()); /* Pretty print with width 80 */
        eprintln!("best expression : {:?}", best_expr.to_string());
        println!("{} {}",current_vector_width,current_vector_width);
        eprintln!("\n===> Final expression depth : {}", rules_2::ast_depth(&best_expr));
        eprintln!("\nCost: {}", best_cost);
        
    }

    
}
