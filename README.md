## Overview

Fully Homomorphic Encryption (FHE) is a technique that makes it possible to perform computations on encrypted data without the need to decrypt it first. While this technique is promising, multiple obstacles have prevented its widespread adoption, including its complexity and slowness. Compilers were developed to simplify the process for non-FHE expert developers by offering a user-friendly frontend to write their programs and optimize them for the best runtime.

CHEHAB is an FHE compiler that offers a simple Domain Specific Language (DSL) to define inputs and outputs and describe the desired computations. CHEHAB abstracts FHE complexities by automating the selection of optimal parameters that can support the generated noise and inserting ciphertext maintenance operations. It also supports multiple optimization techniques like Constant Folding (CF), Common Sub-expression Elimination (CSE), and Term Rewriting System (TRS) as the main optimization technique. Currently, CHEHAB supports only the BFV scheme and targets the SEAL library.

## Example

Here is an example written in our compiler's DSL

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

## CHEHAB Dev Environment Setup
In order to build and use the compiler, you need to have installed:
- Cmake
- GCC and G++ compilers
- SEAL library version 4.1
- Python requirements for the RL framework
- Rust and egg library for equality saturation framework

### 1. Clone the Project Repository
```bash
cd /scratch/<your_user_id>/
git clone https://github.com/Modern-Compilers-Lab/CHEHAB.git
cd CHEHAB
```
📌 This directory contains the `environment.yml` file used to configure the Conda environment.

### 2. Create and Activate the Conda Environment
```bash
conda env create -f environment.yml -n chehabEnv
conda activate chehabEnv
```
### 3. Install Microsoft SEAL (from Source)

```bash
cd /scratch/<your_user_id>/
conda activate chehabEnv
git clone https://github.com/microsoft/SEAL.git
cd SEAL
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX
cmake --build build
cmake --install build
```

## 🚀 Running Benchmark Instances

To build CHEHAB, you need to follow these steps:
1. Navigate to the cloned repository.
```shell
cd CHEHAB
```
2. Build the compiler 
```shell
cmake -S . -B build
cd build
make
```
### Running the benchmarks

Benchmarks codes can be found in the `benchmarks` directory in the repository's main directory, while their executables can be found in the `benchmarks` directory within the `build` directory.

For nearly all benchmarks we have a python script `generate_benchmark_name.py` excluding {polynomials_coyote, max, sort} which is used to generate random input values, run the corresponding benchmark code on the inputs and store and inputs and the outputs in a `file_io_example.txt`.

The goal of this file to have the same input values used when runnning executable FHE code and to check that decrypted result is the same as the one obtained when running python script on plaintext values.


A benchmark is run in two phases. The first execution triggers our compiler to translate the program written in our DSL into a program using FHE native primitives. The second execution corresponds to the concrete homomorphic evaluation. In the following steps, we use the **box blur** benchmark as an example.

1. While in the build directory, navigate to the corresponding benchmark directory.
```shell
cd benchmarks/box_blur
```
2. run python script 
```shell
python3 generate_box_blur.py --slot_count 4 
```

3. run the benchmark to generate FHE code
```shell
./box_blur 1 4 1 0 1 1 1 
```
The general command to execute a benchmark is; 

```shell
./benchmark_name <vectorize_code> <slot_count> <optimization_method> <window> <call_quantifier> <cse> <const_folding>
```


- **vectorized_code:**	(0/1)	to enable vectorized vs scalar code generation  
- **window:**	0	Vectorization window size  
- **call_quantifier:**	(0/1)	to enable performance analysis and metrics  
- **cse:**	(0/1)	to enable Common Subexpression Elimination  
- **slot_count:**	to Specify the input size/dimensions for the benchmark  
- **const_folding:**	(0/1)	to enable constant folding  
- **optimization_method:** (0/1): 1 for RL, 0 for egraphs; selects the optimization process to use.

4. Navigate to the `he` directory, where you can find the created files to build the final executable. This automatically links the generated file with the SEAL library.

```shell
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

This will use the `box_blur_io_example.txt` file, included in the `build` directory to extract the input values, encode them, and then encrypt them for use in the homomorphic evaluation.

