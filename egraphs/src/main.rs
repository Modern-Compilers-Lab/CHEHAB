extern crate clap;

use clap::{App, Arg};
use egraphslib::rules;
fn main() {
    let matches = App::new("Rewriter")
        .arg(
            Arg::with_name("INPUT")
                .help("Sets the input file")
                .required(true)
                .index(1),
        )
        .arg(
            Arg::with_name("slot_count")
                .help("Sets the slot_count")
                .required(true)
                .index(2),
        )
        .arg(
            Arg::with_name("axiomatic")
                .help("If true use axiomatic rules otherwise use specific rules")
                .required(true)
                .index(3),
        )
        .get_matches();

    use std::{env, fs};
    let path = matches.value_of("INPUT").unwrap();

    let slot_count: usize = matches
        .value_of("slot_count")
        .unwrap()
        .parse()
        .expect("Number must be a valid usize");
    let axiomatic_int: usize = matches
        .value_of("axiomatic")
        .unwrap()
        .parse()
        .expect("axiomatic must be a valid usize");
    let axiomatic = axiomatic_int == 1;
    let timeout = env::var("TIMEOUT")
        .ok()
        .and_then(|t| t.parse::<u64>().ok())
        .unwrap_or(300);
    let prog_str = fs::read_to_string(path).expect("Failed to read the input file.");
    let prog = prog_str.parse().unwrap();

    // Run rewriter
    let (cost, best) = rules::run(&prog, timeout, slot_count, axiomatic);
    let binding = best.to_string();
    let best_str: &str = binding.as_ref();
    let processed_best = best_str.replace("(", "( ").replace(")", " )");
    print!("{}", processed_best);
    eprintln!("\nCost: {}", cost);
}
