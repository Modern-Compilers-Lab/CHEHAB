extern crate clap;
use clap::{App, Arg};
use dioslib::*;
fn main() {
    let matches = App::new("Rewriter")
        .arg(
            Arg::with_name("INPUT")
                .help("Sets the input file")
                .required(true)
                .index(1),
        )
        .arg(
            Arg::with_name("no-ac")
                .long("no-ac")
                .help("Disable associativity and commutativity rules"),
        )
        .arg(
            Arg::with_name("no-vec")
                .long("no-vec")
                .help("Disable vector rules"),
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
    let prog = prog_str.parse().unwrap();

    // Rules to disable flags
    let no_ac = matches.is_present("no-ac");
    let no_vec = matches.is_present("no-vec");

    // Run rewriter
    eprintln!(
        "Running egg with timeout {:?}s, width: {:?}",
        timeout,
        config::vector_width()
    );
    let (cost, best) = rules::run(&prog, timeout, no_ac, no_vec);

    println!("{}", best.to_string()); /* Pretty print with width 80 */
    eprintln!("\nCost: {}", cost);
}
