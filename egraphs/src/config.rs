pub const LITERAL: f64 = 0.0;
pub const STRUCTURE: f64 = 2000.0;
pub const VEC_OP: f64 = 1.0;
pub const OP: f64 = 1.0;
pub const ALPHA: f64 = 0.0;
pub const BETA: f64 = 0.0;
pub const GAMMA: f64 = 0.0;
pub const DELTA: f64 = 1.0;
pub const STRUCTURED_WITH_ONE_OUTPUT: usize = 21;
pub const STRUCTURED_WITH_MULTIPLE_OUTPUTS: usize = 11;
pub const UNSTRUCTURED_WITH_ONE_OUTPUT: usize = 10;

// Compile-time vector width
use std::env;
pub fn vector_width() -> usize {
    env::var("VECTOR_WIDTH")
        .ok()
        .and_then(|width_str| width_str.parse().ok())
        .unwrap_or(0)
}
