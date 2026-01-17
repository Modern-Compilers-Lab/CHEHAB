
# CHEHAB: A Fully Homomorphic Encryption Compiler

## 🧠 Overview

Fully Homomorphic Encryption (FHE) allows computations on encrypted data **without decrypting it**, enabling privacy-preserving computation. Despite its potential, FHE adoption remains limited due to challenges such as:

- High computational overhead
- Parameter tuning complexity
- Tedious ciphertext management

**CHEHAB** addresses these challenges through a specialized FHE compiler built with the following goals:

- A **Domain-Specific Language (DSL)** for describing FHE computations.
- **Automatic parameter selection** and ciphertext maintenance.
- Multiple **optimization techniques**:
  - Constant Folding (CF)
  - Common Subexpression Elimination (CSE)
  - Term Rewriting System (TRS) for advanced transformations

Currently, CHEHAB supports the **BFV scheme** and targets the **Microsoft SEAL library**.

---

## 📘 DSL Example

Below is a simple example written in CHEHAB's DSL:

```cpp
#include "fheco/fheco.hpp"
using namespace fheco;

void example()
{
  Ciphertext c0("c0"); 
  Ciphertext c1 = c0 << 1;
  Ciphertext c2 = c0 << 5;
  Ciphertext c3 = c0 << 6;
  Ciphertext c4 = c1 + c0;
  Ciphertext c5 = c2 + c3;
  Ciphertext c6 = c4 + c5;
  c6.set_output("c6");
}
```

---

## ⚙️ Dev Environment Setup

### 🔧 Prerequisites

- GCC and G++ compilers
- CMake
- SEAL library (v4.1)
- Rust (for TRS/e-graph optimizer via `egg`)
- Python dependencies (for RL optimization)

### 1. Clone the Repository

```bash
cd /scratch/<your_user_id>/
git clone https://github.com/Modern-Compilers-Lab/CHEHAB.git
cd CHEHAB
```

This directory contains an `environment.yml` for setting up the Conda environment.

### 2. Create and Activate Conda Environment

```bash
conda env create -f environment.yml -n chehabEnv
conda activate chehabEnv
```

### 3. Install SEAL (Microsoft SEAL Library)

```bash
cd /scratch/<your_user_id>/
git clone https://github.com/microsoft/SEAL.git
cd SEAL
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX
cmake --build build
cmake --install build
```

---

## 🚀 Building and Running Benchmarks

### 🔨 Build the Compiler

```bash 
cd CHEHAB
cmake -S . -B build
cd build
make
```

### 📊 Compiler Directory Structure

- Benchmarks: `benchmarks/<benchmark_name>/`
- Equality saturation framework : `egraphs/`
- Reinforcment learning framework : `RL/`
- Core compiler `src/`

Most benchmarks use a generator script `generate_<benchmark>.py` to:
1. Generate random input values
2. Run reference plaintext computations
3. Save inputs and outputs in `<benchmark>_io_example.txt`
4. The goal is to have the same input values used when runnning executable FHE code and to check that decrypted result is the same as the one obtained when running python script on plaintext values.

A benchmark is run in two phases :  
- The first execution triggers our compiler to translate the program written in our DSL into a program using FHE native primitives. 
- The second execution corresponds to the concrete homomorphic evaluation. In the following steps, we use the **box blur** benchmark as an example.

### 🧪 Run the Benchmark (First Phase)

```bash
cd build/benchmarks/box_blur
python3 generate_box_blur.py --slot_count 4
./box_blur 1 4 1 0 1 1 1
```

#### General Command Format

```bash
./<benchmark> <vectorize_code> <slot_count> <optimization_method> <window> <call_quantifier> <cse> <const_folding>
```

| Argument            | Description                               |
|---------------------|-------------------------------------------|
| `vectorize_code`    | 0/1 - Scalar or vectorized code           |
| `slot_count`        | Number of input slots                     |
| `optimization_method` | 0 for e-graph, 1 for RL                 |
| `window`            | Vectorization window size                 |
| `call_quantifier`   | 0/1 - Enable metric collection            |
| `cse`               | 0/1 - Enable common subexpression elimination |
| `const_folding`     | 0/1 - Enable constant folding             |

---

## 🔐 Final Homomorphic Execution (Second Phase)

1. Navigate to the `he` directory, where you can find the created files to build the final executable. This automatically links the generated file with the SEAL library.

```bash
cd he
mkdir build
cmake -S . -B build
cd build
make
```

3. Within the `build` directory, you will find the `main` executable that triggers the concrete homomorphic evaluation. You only need to execute the `main` file.

```shell
./main
```



