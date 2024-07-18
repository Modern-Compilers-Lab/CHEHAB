// Compile-time vector width, default to 4
use std::env;
pub fn vector_width() -> usize {
    env::var("VECTOR_WIDTH")
        .ok()
        .and_then(|width_str| width_str.parse().ok())
        .unwrap_or(4) // Default to 4 if the variable is not set or parsing fails
}
