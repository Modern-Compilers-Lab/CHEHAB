from langchain_core.prompts import ChatPromptTemplate







def build_veclang_prompt() -> ChatPromptTemplate:
    return ChatPromptTemplate.from_messages([
        ("system", "You are a rigorous validator for VecLang expressions. First ANALYZE then GENERATE. **Enforce structural uniqueness beyond variable renaming.**"),
        ("human", PROMPT),
    ])


PROMPT = """#######################  VecLang Generation Protocol  #######################

You will output **5 structurally unique** `(Vec …)` expressions for RL training.
Every expression **must** honour all rules below.  If any check fails, discard
the draft and regenerate before replying.

──────────────────────────────────────────────────────────────────────────────
1.  Core Vector Form
──────────────────────────────────────────────────────────────────────────────
• Start with `(Vec` and contain **exactly `{vec_size}` sub‑expressions**.  
  Skeleton → `(Vec expr₀ expr₁ … expr₍vec_size‑1₎)`.  
• No nested `(Vec …)` inside a sub‑expression.  
• Sub‑expression depth: **4 ≤ depth ≤ 20**.  

──────────────────────────────────────────────────────────────────────────────
2.  Syntax / Operator Rules (✅ self‑check)
──────────────────────────────────────────────────────────────────────────────
✓ Balanced parentheses.  
✓ Operators: `+` and `*` are strictly binary; `-` may be unary or binary.  
✓ Variables match `[a-z][0-9]_*[0-9]*` (e.g. `in_1_0`, `v3_2`).  
✓ **No numeric literal 0** anywhere.  
✓ Ensure that are No degenerate single‑term parentheses such as `(x)` or `(7)`.
✓ `(x)` or `(7)` are wrong and should not be in a subexpression.


──────────────────────────────────────────────────────────────────────────────
3.  Semantic / Structural Rules (✅)
──────────────────────────────────────────────────────────────────────────────
✓ **Structural uniqueness** after canonicalising (V/C) w.r.t. history/examples.  
✓ **Operation asymmetry**: avoid identical operator trees across siblings.  
✓ Expressions must not be trivially vectorisable; a sequence of rewrite rules
  (see §7) should improve depth or multiplicative depth.

──────────────────────────────────────────────────────────────────────────────
4.  Mandatory Generation Checklist
──────────────────────────────────────────────────────────────────────────────
1. Draft 5 candidate `(Vec …)` lines.  
2. ✅ Count elements == `{vec_size}`.  
3. ✅ Validate parentheses & operator set (§2).  
4. ✅ Compute depths & variable counts.  
5. Canonicalise and check for structural duplicates.  
6. Output the clean expressions—*one per line, no commentary*.
7. At least 3 expressions must have depth >10. Range from moderately simple to very large/deeply nested, resembling Coyote polynomials (§9).
──────────────────────────────────────────────────────────────────────────────
5.  Quick Size Reference
──────────────────────────────────────────────────────────────────────────────
Different computations yield **different numbers of scalar outputs**; `Vec`
length must match that count.

| Kernel | Output shape | Required `Vec` length |
|--------|--------------|-----------------------|
| Hamming Distance | 1 scalar | **1** |
| Linear Regression (batch 4) | 4 scalars | **4** |
| 4 × 4 Matrix Multiply | 16 scalars | **16** |

──────────────────────────────────────────────────────────────────────────────
6.  Worked Example Breakdown
──────────────────────────────────────────────────────────────────────────────
Below are **full VecLang programs** illustrating how real computations map to
`Vec` expressions and why their lengths differ. *Do NOT copy or trivially
rename them.*

⟡ **Union‑Cardinality (size 1)** — computes the number of positions where at least one bit is 1 between two 4‑bit vectors (i.e. the cardinality of their bitwise OR), and returns that count as a single‑lane result.
the mismatches into a single distance value, so the `Vec` has one lane:
```veclang
(Vec (+ (+ (+ ( - ( + v1_0 v2_0 ) ( * v1_0 v2_0 ) ) ( - ( + v1_1 v2_1 ) ( * v1_1 v2_1 ) ) ) ( - ( + v1_2 v2_2 ) ( * v1_2 v2_2 ) ) ) ( - ( + v1_3 v2_3 ) ( * v1_3 v2_3 ) ) ))

````

⟡  *Squared Difference (size 4)* — computes the element‑wise squared error between two 4‑lane vectors, producing a 4‑lane Vec of (v1_i - v2_i) squared:
Each lane computes `v2_i + 2 + 5·v1_i`, giving a length‑4 `Vec`:

```veclang
(Vec ( * ( - v1_0 v2_0 ) ( - v1_0 v2_0 ) ) ( * ( - v1_1 v2_1 ) ( - v1_1 v2_1 ) ) ( * ( - v1_2 v2_2 ) ( - v1_2 v2_2 ) ) ( * ( - v1_3 v2_3 ) ( - v1_3 v2_3 ) ))
``

⟡ **4 × 4 Matrix Addition (size 16)** — produces a 4×4 output matrix.
Each lane is one addition of corresponding elements; outputs are flattened row‑major, hence 16 lanes:

```veclang
(Vec ( + a_0_0 b_0_0 ) ( + a_0_1 b_0_1 ) ( + a_0_2 b_0_2 ) ( + a_0_3 b_0_3 ) ( + a_1_0 b_1_0 ) ( + a_1_1 b_1_1 ) ( + a_1_2 b_1_2 ) ( + a_1_3 b_1_3 ) ( + a_2_0 b_2_0 ) ( + a_2_1 b_2_1 ) ( + a_2_2 b_2_2 ) ( + a_2_3 b_2_3 ) ( + a_3_0 b_3_0 ) ( + a_3_1 b_3_1 ) ( + a_3_2 b_3_2 ) ( + a_3_3 b_3_3 ))
```



──────────────────────────────────────────────────────────────────────────────
7\.  Rewrite Rules Context
──────────────────────────────────────────────────────────────────────────────
Rewrite {{{{ name: "add-0-0+0", searcher: 0, applier: (+ 0 0) }}}}
Rewrite {{{{ name: "add-a-a+0", searcher: ?a, applier: (+ ?a 0) }}}}
Rewrite {{{{ name: "add-a*b-0+a*b", searcher: (* ?a ?b), applier: (+ 0 (* ?a ?b)) }}}}
Rewrite {{{{ name: "add-a-b-0+a-b", searcher: (- ?a ?b), applier: (+ 0 (- ?a ?b)) }}}}
Rewrite {{{{ name: "add--a-0+-a", searcher: (- ?a), applier: (+ 0 (- ?a)) }}}}
Rewrite {{{{ name: "neg-0-0+0", searcher: 0, applier: (- 0) }}}}
Rewrite {{{{ name: "sub-0-0-0", searcher: 0, applier: (- 0 0) }}}}
Rewrite {{{{ name: "sub-a-a-0", searcher: ?a, applier: (- ?a 0) }}}}
Rewrite {{{{ name: "sub-a*b-0-a*b", searcher: (* ?a ?b), applier: (- 0 (* ?a ?b)) }}}}
Rewrite {{{{ name: "sub-a+b-0-a+b", searcher: (+ ?a ?b), applier: (- 0 (+ ?a ?b)) }}}}
Rewrite {{{{ name: "sub--a-0--a", searcher: (- ?a), applier: (- 0 (- ?a)) }}}}
Rewrite {{{{ name: "add-vectorize-2", searcher: (Vec (+ ?a0 ?b0) (+ ?a1 ?b1)), applier: (VecAdd (Vec ?a0 ?a1) (Vec ?b0 ?b1)) }}}}
Rewrite {{{{ name: "add-vectorize-4", searcher: (Vec (+ ?a0 ?b0) (+ ?a1 ?b1) (+ ?a2 ?b2) (+ ?a3 ?b3)), applier: (VecAdd (Vec ?a0 ?a1 ?a2 ?a3) (Vec ?b0 ?b1 ?b2 ?b3)) }}}}
Rewrite {{{{ name: "add-vectorize-8", searcher: (Vec (+ ?a0 ?b0) (+ ?a1 ?b1) (+ ?a2 ?b2) (+ ?a3 ?b3) (+ ?a4 ?b4) (+ ?a5 ?b5) (+ ?a6 ?b6) (+ ?a7 ?b7)), applier: (VecAdd (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7) (Vec ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7)) }}}}
Rewrite {{{{ name: "add-vectorize-16", searcher: (Vec (+ ?a0 ?b0) (+ ?a1 ?b1) (+ ?a2 ?b2) (+ ?a3 ?b3) (+ ?a4 ?b4) (+ ?a5 ?b5) (+ ?a6 ?b6) (+ ?a7 ?b7) (+ ?a8 ?b8) (+ ?a9 ?b9) (+ ?a10 ?b10) (+ ?a11 ?b11) (+ ?a12 ?b12) (+ ?a13 ?b13) (+ ?a14 ?b14) (+ ?a15 ?b15)), applier: (VecAdd (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?a8 ?a9 ?a10 ?a11 ?a12 ?a13 ?a14 ?a15) (Vec ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7 ?b8 ?b9 ?b10 ?b11 ?b12 ?b13 ?b14 ?b15)) }}}}
Rewrite {{{{ name: "add-vectorize-32", searcher: (Vec (+ ?a0 ?b0) (+ ?a1 ?b1) (+ ?a2 ?b2) (+ ?a3 ?b3) (+ ?a4 ?b4) (+ ?a5 ?b5) (+ ?a6 ?b6) (+ ?a7 ?b7) (+ ?a8 ?b8) (+ ?a9 ?b9) (+ ?a10 ?b10) (+ ?a11 ?b11) (+ ?a12 ?b12) (+ ?a13 ?b13) (+ ?a14 ?b14) (+ ?a15 ?b15) (+ ?a16 ?b16) (+ ?a17 ?b17) (+ ?a18 ?b18) (+ ?a19 ?b19) (+ ?a20 ?b20) (+ ?a21 ?b21) (+ ?a22 ?b22) (+ ?a23 ?b23) (+ ?a24 ?b24) (+ ?a25 ?b25) (+ ?a26 ?b26) (+ ?a27 ?b27) (+ ?a28 ?b28) (+ ?a29 ?b29) (+ ?a30 ?b30) (+ ?a31 ?b31)), applier: (VecAdd (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?a8 ?a9 ?a10 ?a11 ?a12 ?a13 ?a14 ?a15 ?a16 ?a17 ?a18 ?a19 ?a20 ?a21 ?a22 ?a23 ?a24 ?a25 ?a26 ?a27 ?a28 ?a29 ?a30 ?a31) (Vec ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7 ?b8 ?b9 ?b10 ?b11 ?b12 ?b13 ?b14 ?b15 ?b16 ?b17 ?b18 ?b19 ?b20 ?b21 ?b22 ?b23 ?b24 ?b25 ?b26 ?b27 ?b28 ?b29 ?b30 ?b31)) }}}}
Rewrite {{{{ name: "rot-add-vectorize-1", searcher: (Vec (+ ?a0 ?b0)), applier: (VecAdd (Vec ?a0 ?b0) (<< (Vec ?a0 ?b0) 1)) }}}}
Rewrite {{{{ name: "rot-add-vectorize-2", searcher: (Vec (+ ?a0 ?b0) (+ ?a1 ?b1)), applier: (VecAdd (Vec ?a0 ?a1 ?b0 ?b1) (<< (Vec ?a0 ?a1 ?b0 ?b1) 2)) }}}}
Rewrite {{{{ name: "rot-add-vectorize-4", searcher: (Vec (+ ?a0 ?b0) (+ ?a1 ?b1) (+ ?a2 ?b2) (+ ?a3 ?b3)), applier: (VecAdd (Vec ?a0 ?a1 ?a2 ?a3 ?b0 ?b1 ?b2 ?b3) (<< (Vec ?a0 ?a1 ?a2 ?a3 ?b0 ?b1 ?b2 ?b3) 4)) }}}}
Rewrite {{{{ name: "rot-add-vectorize-8", searcher: (Vec (+ ?a0 ?b0) (+ ?a1 ?b1) (+ ?a2 ?b2) (+ ?a3 ?b3) (+ ?a4 ?b4) (+ ?a5 ?b5) (+ ?a6 ?b6) (+ ?a7 ?b7)), applier: (VecAdd (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7) (<< (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7) 8)) }}}}
Rewrite {{{{ name: "rot-add-vectorize-16", searcher: (Vec (+ ?a0 ?b0) (+ ?a1 ?b1) (+ ?a2 ?b2) (+ ?a3 ?b3) (+ ?a4 ?b4) (+ ?a5 ?b5) (+ ?a6 ?b6) (+ ?a7 ?b7) (+ ?a8 ?b8) (+ ?a9 ?b9) (+ ?a10 ?b10) (+ ?a11 ?b11) (+ ?a12 ?b12) (+ ?a13 ?b13) (+ ?a14 ?b14) (+ ?a15 ?b15)), applier: (VecAdd (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?a8 ?a9 ?a10 ?a11 ?a12 ?a13 ?a14 ?a15 ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7 ?b8 ?b9 ?b10 ?b11 ?b12 ?b13 ?b14 ?b15) (<< (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?a8 ?a9 ?a10 ?a11 ?a12 ?a13 ?a14 ?a15 ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7 ?b8 ?b9 ?b10 ?b11 ?b12 ?b13 ?b14 ?b15) 16)) }}}}
Rewrite {{{{ name: "sub-vectorize-2", searcher: (Vec (- ?a0 ?b0) (- ?a1 ?b1)), applier: (VecMinus (Vec ?a0 ?a1) (Vec ?b0 ?b1)) }}}}
Rewrite {{{{ name: "sub-vectorize-4", searcher: (Vec (- ?a0 ?b0) (- ?a1 ?b1) (- ?a2 ?b2) (- ?a3 ?b3)), applier: (VecMinus (Vec ?a0 ?a1 ?a2 ?a3) (Vec ?b0 ?b1 ?b2 ?b3)) }}}}
Rewrite {{{{ name: "sub-vectorize-8", searcher: (Vec (- ?a0 ?b0) (- ?a1 ?b1) (- ?a2 ?b2) (- ?a3 ?b3) (- ?a4 ?b4) (- ?a5 ?b5) (- ?a6 ?b6) (- ?a7 ?b7)), applier: (VecMinus (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7) (Vec ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7)) }}}}
Rewrite {{{{ name: "sub-vectorize-16", searcher: (Vec (- ?a0 ?b0) (- ?a1 ?b1) (- ?a2 ?b2) (- ?a3 ?b3) (- ?a4 ?b4) (- ?a5 ?b5) (- ?a6 ?b6) (- ?a7 ?b7) (- ?a8 ?b8) (- ?a9 ?b9) (- ?a10 ?b10) (- ?a11 ?b11) (- ?a12 ?b12) (- ?a13 ?b13) (- ?a14 ?b14) (- ?a15 ?b15)), applier: (VecMinus (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?a8 ?a9 ?a10 ?a11 ?a12 ?a13 ?a14 ?a15) (Vec ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7 ?b8 ?b9 ?b10 ?b11 ?b12 ?b13 ?b14 ?b15)) }}}}
Rewrite {{{{ name: "sub-vectorize-32", searcher: (Vec (- ?a0 ?b0) (- ?a1 ?b1) (- ?a2 ?b2) (- ?a3 ?b3) (- ?a4 ?b4) (- ?a5 ?b5) (- ?a6 ?b6) (- ?a7 ?b7) (- ?a8 ?b8) (- ?a9 ?b9) (- ?a10 ?b10) (- ?a11 ?b11) (- ?a12 ?b12) (- ?a13 ?b13) (- ?a14 ?b14) (- ?a15 ?b15) (- ?a16 ?b16) (- ?a17 ?b17) (- ?a18 ?b18) (- ?a19 ?b19) (- ?a20 ?b20) (- ?a21 ?b21) (- ?a22 ?b22) (- ?a23 ?b23) (- ?a24 ?b24) (- ?a25 ?b25) (- ?a26 ?b26) (- ?a27 ?b27) (- ?a28 ?b28) (- ?a29 ?b29) (- ?a30 ?b30) (- ?a31 ?b31)), applier: (VecMinus (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?a8 ?a9 ?a10 ?a11 ?a12 ?a13 ?a14 ?a15 ?a16 ?a17 ?a18 ?a19 ?a20 ?a21 ?a22 ?a23 ?a24 ?a25 ?a26 ?a27 ?a28 ?a29 ?a30 ?a31) (Vec ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7 ?b8 ?b9 ?b10 ?b11 ?b12 ?b13 ?b14 ?b15 ?b16 ?b17 ?b18 ?b19 ?b20 ?b21 ?b22 ?b23 ?b24 ?b25 ?b26 ?b27 ?b28 ?b29 ?b30 ?b31)) }}}}
Rewrite {{{{ name: "rot-min-vectorize-1", searcher: (Vec (- ?a0 ?b0)), applier: (VecMinus (Vec ?a0 ?b0) (<< (Vec ?a0 ?b0) 1)) }}}}
Rewrite {{{{ name: "rot-min-vectorize-2", searcher: (Vec (- ?a0 ?b0) (- ?a1 ?b1)), applier: (VecMinus (Vec ?a0 ?a1 ?b0 ?b1) (<< (Vec ?a0 ?a1 ?b0 ?b1) 2)) }}}}
Rewrite {{{{ name: "rot-min-vectorize-4", searcher: (Vec (- ?a0 ?b0) (- ?a1 ?b1) (- ?a2 ?b2) (- ?a3 ?b3)), applier: (VecMinus (Vec ?a0 ?a1 ?a2 ?a3 ?b0 ?b1 ?b2 ?b3) (<< (Vec ?a0 ?a1 ?a2 ?a3 ?b0 ?b1 ?b2 ?b3) 4)) }}}}
Rewrite {{{{ name: "rot-min-vectorize-8", searcher: (Vec (- ?a0 ?b0) (- ?a1 ?b1) (- ?a2 ?b2) (- ?a3 ?b3) (- ?a4 ?b4) (- ?a5 ?b5) (- ?a6 ?b6) (- ?a7 ?b7)), applier: (VecMinus (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7) (<< (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7) 8)) }}}}
Rewrite {{{{ name: "rot-min-vectorize-16", searcher: (Vec (- ?a0 ?b0) (- ?a1 ?b1) (- ?a2 ?b2) (- ?a3 ?b3) (- ?a4 ?b4) (- ?a5 ?b5) (- ?a6 ?b6) (- ?a7 ?b7) (- ?a8 ?b8) (- ?a9 ?b9) (- ?a10 ?b10) (- ?a11 ?b11) (- ?a12 ?b12) (- ?a13 ?b13) (- ?a14 ?b14) (- ?a15 ?b15)), applier: (VecMinus (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?a8 ?a9 ?a10 ?a11 ?a12 ?a13 ?a14 ?a15 ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7 ?b8 ?b9 ?b10 ?b11 ?b12 ?b13 ?b14 ?b15) (<< (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?a8 ?a9 ?a10 ?a11 ?a12 ?a13 ?a14 ?a15 ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7 ?b8 ?b9 ?b10 ?b11 ?b12 ?b13 ?b14 ?b15) 16)) }}}}
Rewrite {{{{ name: "mul-0-0*0", searcher: 0, applier: (* 0 0) }}}}
Rewrite {{{{ name: "mul-a-a*1", searcher: ?a, applier: (* ?a 1) }}}}
Rewrite {{{{ name: "mul-a+b-1-a+b", searcher: (+ ?a ?b), applier: (* 1 (+ ?a ?b)) }}}}
Rewrite {{{{ name: "mul-a-b-1-a-b", searcher: (- ?a ?b), applier: (* 1 (- ?a ?b)) }}}}
Rewrite {{{{ name: "mul-vectorize-2", searcher: (Vec (* ?a0 ?b0) (* ?a1 ?b1)), applier: (VecMul (Vec ?a0 ?a1) (Vec ?b0 ?b1)) }}}}
Rewrite {{{{ name: "mul-vectorize-4", searcher: (Vec (* ?a0 ?b0) (* ?a1 ?b1) (* ?a2 ?b2) (* ?a3 ?b3)), applier: (VecMul (Vec ?a0 ?a1 ?a2 ?a3) (Vec ?b0 ?b1 ?b2 ?b3)) }}}}
Rewrite {{{{ name: "mul-vectorize-8", searcher: (Vec (* ?a0 ?b0) (* ?a1 ?b1) (* ?a2 ?b2) (* ?a3 ?b3) (* ?a4 ?b4) (* ?a5 ?b5) (* ?a6 ?b6) (* ?a7 ?b7)), applier: (VecMul (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7) (Vec ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7)) }}}}
Rewrite {{{{ name: "mul-vectorize-16", searcher: (Vec (* ?a0 ?b0) (* ?a1 ?b1) (* ?a2 ?b2) (* ?a3 ?b3) (* ?a4 ?b4) (* ?a5 ?b5) (* ?a6 ?b6) (* ?a7 ?b7) (* ?a8 ?b8) (* ?a9 ?b9) (* ?a10 ?b10) (* ?a11 ?b11) (* ?a12 ?b12) (* ?a13 ?b13) (* ?a14 ?b14) (* ?a15 ?b15)), applier: (VecMul (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?a8 ?a9 ?a10 ?a11 ?a12 ?a13 ?a14 ?a15) (Vec ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7 ?b8 ?b9 ?b10 ?b11 ?b12 ?b13 ?b14 ?b15)) }}}}
Rewrite {{{{ name: "mul-vectorize-32", searcher: (Vec (* ?a0 ?b0) (* ?a1 ?b1) (* ?a2 ?b2) (* ?a3 ?b3) (* ?a4 ?b4) (* ?a5 ?b5) (* ?a6 ?b6) (* ?a7 ?b7) (* ?a8 ?b8) (* ?a9 ?b9) (* ?a10 ?b10) (* ?a11 ?b11) (* ?a12 ?b12) (* ?a13 ?b13) (* ?a14 ?b14) (* ?a15 ?b15) (* ?a16 ?b16) (* ?a17 ?b17) (* ?a18 ?b18) (* ?a19 ?b19) (* ?a20 ?b20) (* ?a21 ?b21) (* ?a22 ?b22) (* ?a23 ?b23) (* ?a24 ?b24) (* ?a25 ?b25) (* ?a26 ?b26) (* ?a27 ?b27) (* ?a28 ?b28) (* ?a29 ?b29) (* ?a30 ?b30) (* ?a31 ?b31)), applier: (VecMul (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?a8 ?a9 ?a10 ?a11 ?a12 ?a13 ?a14 ?a15 ?a16 ?a17 ?a18 ?a19 ?a20 ?a21 ?a22 ?a23 ?a24 ?a25 ?a26 ?a27 ?a28 ?a29 ?a30 ?a31) (Vec ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7 ?b8 ?b9 ?b10 ?b11 ?b12 ?b13 ?b14 ?b15 ?b16 ?b17 ?b18 ?b19 ?b20 ?b21 ?b22 ?b23 ?b24 ?b25 ?b26 ?b27 ?b28 ?b29 ?b30 ?b31)) }}}}
Rewrite {{{{ name: "rot-mul-vectorize-1", searcher: (Vec (* ?a0 ?b0)), applier: (VecMul (Vec ?a0 ?b0) (<< (Vec ?a0 ?b0) 1)) }}}}
Rewrite {{{{ name: "rot-mul-vectorize-2", searcher: (Vec (* ?a0 ?b0) (* ?a1 ?b1)), applier: (VecMul (Vec ?a0 ?a1 ?b0 ?b1) (<< (Vec ?a0 ?a1 ?b0 ?b1) 2)) }}}}
Rewrite {{{{ name: "rot-mul-vectorize-4", searcher: (Vec (* ?a0 ?b0) (* ?a1 ?b1) (* ?a2 ?b2) (* ?a3 ?b3)), applier: (VecMul (Vec ?a0 ?a1 ?a2 ?a3 ?b0 ?b1 ?b2 ?b3) (<< (Vec ?a0 ?a1 ?a2 ?a3 ?b0 ?b1 ?b2 ?b3) 4)) }}}}
Rewrite {{{{ name: "rot-mul-vectorize-8", searcher: (Vec (* ?a0 ?b0) (* ?a1 ?b1) (* ?a2 ?b2) (* ?a3 ?b3) (* ?a4 ?b4) (* ?a5 ?b5) (* ?a6 ?b6) (* ?a7 ?b7)), applier: (VecMul (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7) (<< (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7) 8)) }}}}
Rewrite {{{{ name: "rot-mul-vectorize-16", searcher: (Vec (* ?a0 ?b0) (* ?a1 ?b1) (* ?a2 ?b2) (* ?a3 ?b3) (* ?a4 ?b4) (* ?a5 ?b5) (* ?a6 ?b6) (* ?a7 ?b7) (* ?a8 ?b8) (* ?a9 ?b9) (* ?a10 ?b10) (* ?a11 ?b11) (* ?a12 ?b12) (* ?a13 ?b13) (* ?a14 ?b14) (* ?a15 ?b15)), applier: (VecMul (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?a8 ?a9 ?a10 ?a11 ?a12 ?a13 ?a14 ?a15 ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7 ?b8 ?b9 ?b10 ?b11 ?b12 ?b13 ?b14 ?b15) (<< (Vec ?a0 ?a1 ?a2 ?a3 ?a4 ?a5 ?a6 ?a7 ?a8 ?a9 ?a10 ?a11 ?a12 ?a13 ?a14 ?a15 ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7 ?b8 ?b9 ?b10 ?b11 ?b12 ?b13 ?b14 ?b15) 16)) }}}}
Rewrite {{{{ name: "neg-vectorize-2", searcher: (Vec (- ?b0) (- ?b1)), applier: (VecNeg (Vec ?b0 ?b1)) }}}}
Rewrite {{{{ name: "neg-vectorize-4", searcher: (Vec (- ?b0) (- ?b1) (- ?b2) (- ?b3)), applier: (VecNeg (Vec ?b0 ?b1 ?b2 ?b3)) }}}}
Rewrite {{{{ name: "neg-vectorize-8", searcher: (Vec (- ?b0) (- ?b1) (- ?b2) (- ?b3) (- ?b4) (- ?b5) (- ?b6) (- ?b7)), applier: (VecNeg (Vec ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7)) }}}}
Rewrite {{{{ name: "neg-vectorize-16", searcher: (Vec (- ?b0) (- ?b1) (- ?b2) (- ?b3) (- ?b4) (- ?b5) (- ?b6) (- ?b7) (- ?b8) (- ?b9) (- ?b10) (- ?b11) (- ?b12) (- ?b13) (- ?b14) (- ?b15)), applier: (VecNeg (Vec ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7 ?b8 ?b9 ?b10 ?b11 ?b12 ?b13 ?b14 ?b15)) }}}}
Rewrite {{{{ name: "neg-vectorize-32", searcher: (Vec (- ?b0) (- ?b1) (- ?b2) (- ?b3) (- ?b4) (- ?b5) (- ?b6) (- ?b7) (- ?b8) (- ?b9) (- ?b10) (- ?b11) (- ?b12) (- ?b13) (- ?b14) (- ?b15) (- ?b16) (- ?b17) (- ?b18) (- ?b19) (- ?b20) (- ?b21) (- ?b22) (- ?b23) (- ?b24) (- ?b25) (- ?b26) (- ?b27) (- ?b28) (- ?b29) (- ?b30) (- ?b31)), applier: (VecNeg (Vec ?b0 ?b1 ?b2 ?b3 ?b4 ?b5 ?b6 ?b7 ?b8 ?b9 ?b10 ?b11 ?b12 ?b13 ?b14 ?b15 ?b16 ?b17 ?b18 ?b19 ?b20 ?b21 ?b22 ?b23 ?b24 ?b25 ?b26 ?b27 ?b28 ?b29 ?b30 ?b31)) }}}}
Rewrite {{{{ name: "assoc-balan-add-1", searcher: (VecAdd ?x (VecAdd ?y (VecAdd ?z ?t))), applier: (VecAdd (VecAdd ?x ?y) (VecAdd ?z ?t)) }}}}
Rewrite {{{{ name: "assoc-balan-add-2", searcher: (VecAdd ?x (VecAdd (VecAdd ?z ?t) ?y)), applier: (VecAdd (VecAdd ?x ?z) (VecAdd ?t ?y)) }}}}
Rewrite {{{{ name: "assoc-balan-add-3", searcher: (VecAdd (VecAdd (VecAdd ?x ?y) ?z) ?t), applier: (VecAdd (VecAdd ?x ?y) (VecAdd ?z ?t)) }}}}
Rewrite {{{{ name: "assoc-balan-add-4", searcher: (VecAdd (VecAdd ?x (VecAdd ?y ?z)) ?t), applier: (VecAdd (VecAdd ?x ?y) (VecAdd ?z ?t)) }}}}
Rewrite {{{{ name: "assoc-balan-min-1", searcher: (VecMinus ?x (VecMinus ?y (VecMinus ?z ?t))), applier: (VecMinus (VecMinus ?x ?y) (VecMinus ?z ?t)) }}}}
Rewrite {{{{ name: "assoc-balan-min-2", searcher: (VecMinus ?x (VecMinus (VecMinus ?z ?t) ?y)), applier: (VecMinus (VecMinus ?x ?z) (VecMinus ?t ?y)) }}}}
Rewrite {{{{ name: "assoc-balan-min-3", searcher: (VecMinus (VecMinus (VecMinus ?x ?y) ?z) ?t), applier: (VecMinus (VecMinus ?x ?y) (VecMinus ?z ?t)) }}}}
Rewrite {{{{ name: "assoc-balan-min-4", searcher: (VecMinus (VecMinus ?x (VecMinus ?y ?z)) ?t), applier: (VecMinus (VecMinus ?x ?y) (VecMinus ?z ?t)) }}}}
Rewrite {{{{ name: "assoc-balan-mul-1", searcher: (VecMul ?x (VecMul ?y (VecMul ?z ?t))), applier: (VecMul (VecMul ?x ?y) (VecMul ?z ?t)) }}}}
Rewrite {{{{ name: "assoc-balan-mul-2", searcher: (VecMul ?x (VecMul (VecMul ?z ?t) ?y)), applier: (VecMul (VecMul ?x ?z) (VecMul ?t ?y)) }}}}
Rewrite {{{{ name: "assoc-balan-mul-3", searcher: (VecMul (VecMul (VecMul ?x ?y) ?z) ?t), applier: (VecMul (VecMul ?x ?y) (VecMul ?z ?t)) }}}}
Rewrite {{{{ name: "assoc-balan-mul-4", searcher: (VecMul (VecMul ?x (VecMul ?y ?z)) ?t), applier: (VecMul (VecMul ?x ?y) (VecMul ?z ?t)) }}}}
Rewrite {{{{ name: "assoc-balan-mul-5", searcher: (VecMul ?x (VecMul (VecMul ?y ?z) ?t)), applier: (VecMul (VecMul ?x ?y) (VecMul ?z ?t)) }}}}
Rewrite {{{{ name: "assoc-balan-mul-6", searcher: (VecMul ?x (VecMul (VecMul ?y ?z) ?t)), applier: (VecMul (VecMul ?x ?y) (VecMul ?z ?t)) }}}}
Rewrite {{{{ name: "assoc-balan-add-mul-1", searcher: (VecAdd (VecAdd (VecAdd (VecMul ?c1 ?c2) (VecMul ?d1 ?d2)) (VecMul ?b1 ?b2)) (VecMul ?a1 ?a2)), applier: (VecAdd (VecAdd (VecMul ?a1 ?a2) (VecMul ?b1 ?b2)) (VecAdd (VecMul ?c1 ?c2) (VecMul ?d1 ?d2))) }}}}
Rewrite {{{{ name: "assoc-balan-add-mul-2", searcher: (VecAdd (VecMul ?a1 ?a2) (VecAdd (VecMul ?b1 ?b2) (VecAdd (VecMul ?c1 ?c2) (VecMul ?d1 ?d2)))), applier: (VecAdd (VecAdd (VecMul ?a1 ?a2) (VecMul ?b1 ?b2)) (VecAdd (VecMul ?c1 ?c2) (VecMul ?d1 ?d2))) }}}}
Rewrite {{{{ name: "assoc-balan-add-mul-3", searcher: (VecAdd (VecAdd (VecMul ?a1 ?a2) (VecAdd (VecMul ?b1 ?b2) (VecMul ?c1 ?c2))) (VecMul ?d1 ?d2)), applier: (VecAdd (VecAdd (VecMul ?a1 ?a2) (VecMul ?b1 ?b2)) (VecAdd (VecMul ?c1 ?c2) (VecMul ?d1 ?d2))) }}}}
Rewrite {{{{ name: "assoc-balan-add-mul-4", searcher: (VecAdd (VecAdd (VecMul ?a ?b) ?c) ?d), applier: (VecAdd (VecMul ?a ?b) (VecAdd ?c ?d)) }}}}
Rewrite {{{{ name: "assoc-balan-add-mul-5", searcher: (VecAdd ?a (VecAdd ?b (VecMul ?c ?d))), applier: (VecAdd (VecAdd ?a ?b) (VecMul ?c ?d)) }}}}
Rewrite {{{{ name: "distribute-mul-over-add-1", searcher: (VecMul ?a (VecAdd ?b ?c)), applier: (VecAdd (VecMul ?a ?b) (VecMul ?a ?c)) }}}}
Rewrite {{{{ name: "assoc-balan-add-min-1", searcher: (VecAdd (VecAdd (VecAdd (VecMinus ?c1 ?c2) (VecMinus ?d1 ?d2)) (VecMinus ?b1 ?b2)) (VecMinus ?a1 ?a2)), applier: (VecAdd (VecAdd (VecMinus ?a1 ?a2) (VecMinus ?b1 ?b2)) (VecAdd (VecMinus ?c1 ?c2) (VecMinus ?d1 ?d2))) }}}}
Rewrite {{{{ name: "assoc-balan-add-min-2", searcher: (VecAdd (VecMinus ?a1 ?a2) (VecAdd (VecMinus ?b1 ?b2) (VecAdd (VecMinus ?c1 ?c2) (VecMinus ?d1 ?d2)))), applier: (VecAdd (VecAdd (VecMinus ?a1 ?a2) (VecMinus ?b1 ?b2)) (VecAdd (VecMinus ?c1 ?c2) (VecMinus ?d1 ?d2))) }}}}
Rewrite {{{{ name: "assoc-balan-add-min-3", searcher: (VecAdd (VecAdd (VecMinus ?a1 ?a2) (VecAdd (VecMinus ?b1 ?b2) (VecMinus ?c1 ?c2))) (VecMinus ?d1 ?d2)), applier: (VecAdd (VecAdd (VecMinus ?a1 ?a2) (VecMinus ?b1 ?b2)) (VecAdd (VecMinus ?c1 ?c2) (VecMinus ?d1 ?d2))) }}}}
Rewrite {{{{ name: "assoc-balan-min-mul-1", searcher: (VecMinus (VecMinus (VecMinus (VecMul ?c1 ?c2) (VecMul ?d1 ?d2)) (VecMul ?b1 ?b2)) (VecMul ?a1 ?a2)), applier: (VecMinus (VecMinus (VecMul ?a1 ?a2) (VecMul ?b1 ?b2)) (VecAdd (VecMul ?c1 ?c2) (VecMul ?d1 ?d2))) }}}}
Rewrite {{{{ name: "assoc-balan-min-mul-2", searcher: (VecMinus (VecMul ?a1 ?a2) (VecMinus (VecMul ?b1 ?b2) (VecMinus (VecMul ?c1 ?c2) (VecMul ?d1 ?d2)))), applier: (VecMinus (VecMinus (VecMul ?a1 ?a2) (VecMul ?b1 ?b2)) (VecMinus (VecMul ?c1 ?c2) (VecMul ?d1 ?d2))) }}}}
Rewrite {{{{ name: "assoc-balan-min-mul-3", searcher: (VecMinus (VecMinus (VecMul ?a1 ?a2) (VecMinus (VecMul ?b1 ?b2) (VecMul ?c1 ?c2))) (VecMul ?d1 ?d2)), applier: (VecMinus (VecMinus (VecMul ?a1 ?a2) (VecMul ?b1 ?b2)) (VecMinus (VecMul ?c1 ?c2) (VecMul ?d1 ?d2))) }}}}
Rewrite {{{{ name: "simplify-sub-negate", searcher: (VecMinus ?x (VecNeg ?y)), applier: (VecAdd ?x ?y) }}}}
Rewrite {{{{ name: "simplify-sub-negate-1", searcher: (VecAdd ?x (VecNeg ?y)), applier: (VecMinus ?x ?y) }}}}
Rewrite {{{{ name: "simplify-sub-negate-1-2", searcher: (VecAdd (VecNeg ?y) ?x), applier: (VecMinus ?x ?y) }}}}
Rewrite {{{{ name: "simplify-add-mul-negate-1", searcher: (VecAdd (VecMul ?x (VecNeg ?y)) ?z), applier: (VecMinus ?z (VecMul ?x ?y)) }}}}
Rewrite {{{{ name: "simplify-add-mul-negate-2", searcher: (VecAdd (VecMul (VecNeg ?y) ?x) ?z), applier: (VecMinus ?z (VecMul ?x ?y)) }}}}
Rewrite {{{{ name: "simplify-add-mul-negate-3", searcher: (VecAdd ?z (VecMul ?x (VecNeg ?y))), applier: (VecMinus ?z (VecMul ?x ?y)) }}}}
Rewrite {{{{ name: "simplify-add-mul-negate-4", searcher: (VecAdd ?z (VecMul (VecNeg ?y) ?x)), applier: (VecMinus ?z (VecMul ?y ?x)) }}}}
Rewrite {{{{ name: "simplify-sub-mul-negate-1", searcher: (VecMinus ?z (VecMul ?x (VecNeg ?y))), applier: (VecAdd ?z (VecMul ?x ?y)) }}}}
Rewrite {{{{ name: "simplify-sub-mul-negate-2", searcher: (VecMinus ?z (VecMul (VecNeg ?y) ?x)), applier: (VecAdd ?z (VecMul ?x ?y)) }}}}
Rewrite {{{{ name: "simplify-add-negate-2-1", searcher: (VecAdd ?x (VecMinus (VecNeg ?y) ?z)), applier: (VecMinus ?x (VecAdd ?x ?y)) }}}}
Rewrite {{{{ name: "simplify-add-negate-2-2", searcher: (VecAdd (VecMinus ?z (VecNeg ?y)) ?x), applier: (VecMinus ?x (VecAdd ?x ?y)) }}}}
Rewrite {{{{ name: "comm-factor-1", searcher: (+ (* ?a0 ?b0) (* ?a0 ?c0)), applier: (* ?a0 (+ ?b0 ?c0)) }}}}
Rewrite {{{{ name: "comm-factor-2", searcher: (+ (* ?b0 ?a0) (* ?c0 ?a0)), applier: (* ?a0 (+ ?b0 ?c0)) }}}}
Rewrite {{{{ name: "neg-vectorize-1", searcher: (Vec (- ?b0)), applier: (VecNeg (Vec ?b0)) }}}}




──────────────────────────────────────────────────────────────────────────────
8.  Final Output Contract
──────────────────────────────────────────────────────────────────────────────
• Produce **exactly 5** valid, structurally‑distinct `(Vec …)` expressions.
• At least 3 of the generate expressions **Must have a depth > 6**.
• The Generated expressions where they match vectorization rules , or rules that enables vectorization.
• The Generated expressions must match real world computations and programs or similair **not fully random computations**.
• One expression **per line**, raw text—no numbering, comments, or styling.
• Range them from moderately simple to deeply nested.
• If any candidate breaks §§1–3, discard & regenerate before responding."""