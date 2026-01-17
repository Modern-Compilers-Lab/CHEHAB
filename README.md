
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
- SEAL library (v4.1.0)
- Rust (for TRS/e-graph optimizer via `egg`)
- Python dependencies (for RL optimization)


This directory contains an `environment.yml` for setting up the Conda environment.

### 1. Create and Activate Conda Environment

```bash
conda env create -f environment.yml -n chehabEnv
conda activate chehabEnv
cd RL/pytrs
pip3 install -e . # To install the pytrs package
```

### 2. Install SEAL (Microsoft SEAL Library)

```bash
cd /scratch/<your_user_id>/
git clone https://github.com/microsoft/SEAL.git
cd SEAL
git checkout v4.1.1   

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DSEAL_USE_CXX17=ON -DSEAL_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX="$CONDA_PREFIX" -G Ninja
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
rm -r build
mkdir build
cmake -S . -B build
cd build
make
```

3. Within the `build` directory, you will find the `main` executable that triggers the concrete homomorphic evaluation. You only need to execute the `main` file.

```shell
./main
```

---

## Docker (recommended)

This repository includes Docker/Compose workflows that install Microsoft SEAL, build dependencies, and the Python environment inside the container.

### 1) Interactive shell (compiler + benchmarks)
From the repo root:

```bash
docker compose build chehab-main
docker compose run --rm -it chehab-main /bin/bash
```

Inside the container, ensure the Conda environment is active (it is auto-activated for interactive `bash`, but you can run the following if needed):

```bash
source /opt/conda/etc/profile.d/conda.sh
conda activate chehabEnv
```

Run the benchmark sweep (writes CSV results):

```bash
python run_benchmarks.py
```

Results are written under `results/` (and are available on the host via the bind mount configured in `docker-compose.yml`).

### 2) Web UI (optional)
Start the web service and open the browser UI:

```bash
docker compose build chehab-demo
docker compose up chehab-demo
```

Then browse to `http://localhost:8000`.

### 3) Generate plots from CSV results
After producing a CSV (e.g., `results/results_RL.csv`), generate plots:

```bash
python results/generate_graphs.py --metric exec --csv results/results_RL.csv --label "CHEHAB RL" --output results/exec_time.png
python results/generate_graphs.py --metric compile --csv results/results_RL.csv --label "CHEHAB RL" --output results/compile_time.png
python results/generate_graphs.py --metric noise --csv results/results_RL.csv --label "CHEHAB RL" --output results/noise_budget.png
```


