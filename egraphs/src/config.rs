pub const LITERAL: usize = 0;
pub const STRUCTURE: usize = 2000;
pub const VEC_OP: usize = 1;
pub const OP: usize = 1;
// Compile-time vector width
use std::env;
pub fn vector_width() -> usize {
    env::var("VECTOR_WIDTH")
        .ok()
        .and_then(|width_str| width_str.parse().ok())
        .unwrap_or(0)
}
