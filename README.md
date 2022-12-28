## Overview

FHECO is a compiler for FHE applications. FHECO provides a DSL embedded in C++ as well as various optimizations.

### Example

The following is an example of an FHE prgram written with FHECO.

```cpp
// C++ code with a Tiramisu expression.
#include "fhecompiler.hpp"
#include <bits/stdc++.h>

using namespace std;

std::vector<int64_t> flatten_image(const std::vector<vector<int64_t>> img)
{
  // this is the encoding procedure
  std::vector<int64_t> encoded_image;
  for (auto &row : img)
  {
    for (auto &e : row)
      encoded_image.push_back(e);
  }
  return encoded_image;
}

fhecompiler::Ciphertext sum_all_slots(fhecompiler::Ciphertext &x)
{
  // number of slots needs to be know by the user
  // we assume here that we have 5 slots in x
  fhecompiler::Ciphertext result("sum_result", fhecompiler::VarType::temp);
  result = x;
  x <<= 1;
  result += x;
  x <<= 1;
  result += x;
  x <<= 1;
  result += x;
  x <<= 1;
  result += x;
  // result of sum will be in the first slot
  return result;
}

fhecompiler::Ciphertext dotProduct(fhecompiler::Ciphertext &ct1, fhecompiler::Ciphertext &ct2)
{
  fhecompiler::Ciphertext simd_product = ct1 * ct2;
  return sum_all_slots(simd_product);
}

fhecompiler::Ciphertext simple_sharpening_filter(fhecompiler::Ciphertext &img, int n)
{
  fhecompiler::Ciphertext r0 = img * -8;
  fhecompiler::Ciphertext r1 = img << -n - 1;
  fhecompiler::Ciphertext r2 = img << -n;
  fhecompiler::Ciphertext r3 = img << -n + 1;
  fhecompiler::Ciphertext r4 = img << -1;
  fhecompiler::Ciphertext r5 = img << 1;
  fhecompiler::Ciphertext r6 = img << n - 1;
  fhecompiler::Ciphertext r7 = img << n;
  fhecompiler::Ciphertext r8 = img << n + 1;
  return 2 * img - (r0 + r1 + r2 + r3 + r4 + r5 + r6 + r7 + r8);
}

int main()
{
  try
  {

    fhecompiler::init("test1", 0, 4096, fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    fhecompiler::Ciphertext output1("output1", fhecompiler::VarType::output);

    params_selector::EncryptionParameters params;

    params.set_coef_modulus({50, 50, 50});
    params.set_plaintext_modulus(786433);
    params.set_polynomial_modulus_degree(4096 * 2);

    std::vector<vector<int64_t>> img_clear = {{1, 2, 3, 4}, {5, 6, 7, 8}, {1, 2, 4, 7}, {12, 1, 2, 2}};

    // flatten image
    std::vector<int64_t> img_encoded = flatten_image(img_clear);

    fhecompiler::Plaintext img_plain(img_encoded);

    fhecompiler::Ciphertext img_cipher = fhecompiler::Ciphertext::encrypt(img_plain);

    fhecompiler::Ciphertext output3("output3", fhecompiler::VarType::output);
    output3 = simple_sharpening_filter(img_cipher, 4);

    fhecompiler::compile("test1.hpp", &params);
  }
  catch (const char *message)
  {
    std::cout << message << "\n";
  }
  return 0;
}
```

## Building FHECO Manually

#### Prerequisites

###### Required
1) [CMake](https://cmake.org/): version 3.22.1 or greater.

2) [Autoconf](https://www.gnu.org/software/autoconf/) and [libtool](https://www.gnu.org/software/libtool/).
  
###### Optional

#### Building

## Getting Started

## Run Tests
