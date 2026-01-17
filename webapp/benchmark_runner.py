import json
import os
import re
import subprocess
import shutil
from dataclasses import dataclass, asdict
from functools import lru_cache
from pathlib import Path
from typing import Dict, List, Optional, Tuple

REPO_ROOT = Path(__file__).resolve().parent.parent
BUILD_DIR = REPO_ROOT / "build"
DEFAULT_BENCHMARKS = [
    "box_blur",
    "dot_product",
    "gx_kernel",
    "gy_kernel",
    "hamming_dist",
    "l2_distance",
    "lin_reg",
    "matrix_mul",
    "max",
    "poly_derivative",
    "polynomials_coyote",
    "poly_reg",
    "roberts_cross",
    "sort",
]
POLY_TREE_DIR = REPO_ROOT / "benchmarks" / "polynomials_coyote" / "polynomial_trees"
POLY_TREE_PATTERN = re.compile(r"tree_(?P<regime>[0-9]+-[0-9]+)_(?P<depth>\d+)_(?P<iteration>\d+)\.txt")
ALLOWED_SLOT_COUNTS = [2, 4, 8, 16, 32]
ANSI_ESCAPE = re.compile(r"\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])")


@dataclass
class RunParams:
    benchmark: str
    slot_count: int
    optimization_method: int = 1  # 0 = e-graph, 1 = RL
    vectorize_code: int = 1
    window: int = 0
    call_quantifier: int = 1
    cse: int = 1
    const_folding: int = 1
    # Polynomials-specific knobs
    depth: int = 5
    iteration: int = 1
    regime: str = "50-50"


@dataclass
class RunResult:
    benchmark: str
    slot_count: int
    optimization_method: int
    compile_ms: Optional[float]
    depth: Optional[int]
    multiplicative_depth: Optional[int]
    execution_ms: Optional[float]
    remaining_noise_budget: Optional[int]
    initial_expr: Optional[str]
    optimized_expr: Optional[str]
    vector_sizes: List[int]
    logs: List[Dict[str, str]]

    def to_json(self) -> Dict:
        data = asdict(self)
        return data


class BenchmarkRunError(Exception):
    def __init__(self, message: str, logs: Optional[List[Dict[str, str]]] = None) -> None:
        super().__init__(message)
        self.logs = logs or []


def _run_logged(
    step: str,
    cmd: List[str],
    cwd: Path,
    logs: List[Dict[str, str]],
    timeout: Optional[int] = None,
    env: Optional[Dict[str, str]] = None,
) -> Tuple[str, str]:
    if env is None:
        env = os.environ.copy()
    proc = subprocess.run(
        cmd,
        cwd=str(cwd),
        env=env,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        timeout=timeout,
    )
    clean_out = ANSI_ESCAPE.sub("", proc.stdout or "")
    clean_err = ANSI_ESCAPE.sub("", proc.stderr or "")
    logs.append({"step": step, "stdout": clean_out, "stderr": clean_err})
    if proc.returncode != 0:
        raise BenchmarkRunError(
            f"{step} failed (exit {proc.returncode}). Check logs for stdout/stderr.",
            logs=logs.copy(),
    )
    return clean_out, clean_err


@lru_cache(maxsize=1)
def list_polynomial_options() -> Dict[str, object]:
    """
    Discover available polynomial configurations from the polynomial_trees folder.
    Returns a dict with sorted depths, iterations, regimes, combinations, and a default combo.
    """
    depths = set()
    iterations = set()
    regimes = set()
    combinations: List[Dict[str, object]] = []
    if POLY_TREE_DIR.exists():
        for path in POLY_TREE_DIR.glob("tree_*_*_*.txt"):
            match = POLY_TREE_PATTERN.match(path.name)
            if not match:
                continue
            regime = match.group("regime")
            depth = int(match.group("depth"))
            iteration = int(match.group("iteration"))
            depths.add(depth)
            iterations.add(iteration)
            regimes.add(regime)
            combinations.append({"regime": regime, "depth": depth, "iteration": iteration})

    def _default_combo() -> Dict[str, object]:
        if combinations:
            combinations.sort(key=lambda c: (c["depth"], c["iteration"], c["regime"]))
            return combinations[0]
        return {"regime": "50-50", "depth": 5, "iteration": 1}

    default_combo = _default_combo()
    return {
        "depths": sorted(depths) if depths else [default_combo["depth"]],
        "iterations": sorted(iterations) if iterations else [default_combo["iteration"]],
        "regimes": sorted(regimes) if regimes else [default_combo["regime"]],
        "combinations": combinations,
        "default": default_combo,
    }


def _ensure_configured(logs: List[Dict[str, str]]) -> None:
    if (BUILD_DIR / "CMakeCache.txt").exists():
        return
    env = _system_compiler_env()
    _run_logged(
        "cmake configure",
        ["cmake", "-S", ".", "-B", str(BUILD_DIR)],
        REPO_ROOT,
        logs,
        env=env,
    )


def _build_benchmark(benchmark: str, logs: List[Dict[str, str]]) -> None:
    _run_logged(
        f"build {benchmark}",
        ["cmake", "--build", str(BUILD_DIR), "--target", benchmark],
        REPO_ROOT,
        logs,
    )


def _maybe_generate_inputs(benchmark_dir: Path, benchmark: str, slot_count: int, logs: List[Dict[str, str]]) -> None:
    generator = benchmark_dir / f"generate_{benchmark}.py"
    # fall back to source tree if the script was not copied into build/
    if not generator.exists():
        src_generator = REPO_ROOT / "benchmarks" / benchmark / f"generate_{benchmark}.py"
        if not src_generator.exists():
            src_io_file = REPO_ROOT / "benchmarks" / benchmark / "fhe_io_example.txt"
            if src_io_file.exists():
                benchmark_dir.mkdir(parents=True, exist_ok=True)
                shutil.copy(src_io_file, benchmark_dir / "fhe_io_example.txt")
                logs.append(
                    {
                        "step": "generate inputs",
                        "stdout": f"Copied {src_io_file} into {benchmark_dir}",
                        "stderr": "",
                    }
                )
            else:
                logs.append({"step": "generate inputs", "stdout": "", "stderr": "generator script not found"})
            return
        generator = src_generator
    _run_logged(
        "generate inputs",
        ["python3", str(generator), "--slot_count", str(slot_count)],
        benchmark_dir,
        logs,
    )


def _require_file(path: Path, step: str, logs: List[Dict[str, str]]) -> None:
    if not path.exists():
        msg = f"Required file missing: {path}"
        logs.append({"step": step, "stdout": "", "stderr": msg})
        raise BenchmarkRunError(msg, logs=logs.copy())
    logs.append({"step": step, "stdout": f"Found {path}", "stderr": ""})


def _parse_compile_metrics(output: str) -> Tuple[Optional[float], Optional[int], Optional[int]]:
    compile_ms = None
    depth = None
    multiplicative_depth = None

    ms_match = re.search(r"([0-9]*\.?[0-9]+)\s*ms", output)
    if ms_match:
        try:
            compile_ms = float(ms_match.group(1))
        except ValueError:
            compile_ms = None

    depth_match = re.search(r"max:\s*\((\d+),\s*(\d+)\)", output)
    if depth_match:
        depth = int(depth_match.group(1))
        multiplicative_depth = int(depth_match.group(2))

    return compile_ms, depth, multiplicative_depth


def _parse_execution_metrics(output: str) -> Tuple[Optional[float], Optional[int]]:
    execution_ms = None
    remaining_noise = None

    exec_match = re.search(r"execution_time_\(ms\):\s*([0-9]*\.?[0-9]+)", output)
    if exec_match:
        execution_ms = float(exec_match.group(1))

    noise_match = re.search(r"Remaining_noise_budget:\s*([0-9]+)", output)
    if noise_match:
        remaining_noise = int(noise_match.group(1))

    return execution_ms, remaining_noise


def _read_expr_files(benchmark_dir: Path) -> Tuple[Optional[str], Optional[str], List[int]]:
    """
    The compiler emits expression.txt/vectorized_code.txt one level above the benchmark
    (build/benchmarks/) while the runner expects them inside build/benchmarks/<bench>/.
    Look in both places and copy down into the benchmark folder if found only in the parent.
    """
    candidates = [
        (benchmark_dir / "expression.txt", benchmark_dir / "vectorized_code.txt"),
        (benchmark_dir.parent / "expression.txt", benchmark_dir.parent / "vectorized_code.txt"),
    ]

    expr_path = optimized_path = None
    for expr_candidate, opt_candidate in candidates:
        if expr_candidate.exists() or opt_candidate.exists():
            expr_path = expr_candidate
            optimized_path = opt_candidate
            break

    initial_expr = None
    optimized_expr = None
    vector_sizes: List[int] = []

    if expr_path and expr_path.exists():
        initial_expr = expr_path.read_text().strip()
        # If it was found in the parent, copy it into the benchmark folder for the UI.
        target_expr = benchmark_dir / "expression.txt"
        if expr_path != target_expr:
            try:
                shutil.copy(expr_path, target_expr)
            except Exception:
                pass

    if optimized_path and optimized_path.exists():
        lines = optimized_path.read_text().strip().splitlines()
        if lines:
            optimized_expr = lines[0].strip()
        if len(lines) > 1:
            try:
                vector_sizes = [int(x) for x in lines[1].split() if x.strip().isdigit()]
            except ValueError:
                vector_sizes = []
        target_opt = benchmark_dir / "vectorized_code.txt"
        if optimized_path != target_opt:
            try:
                shutil.copy(optimized_path, target_opt)
            except Exception:
                pass

    return initial_expr, optimized_expr, vector_sizes


def _stage_he_inputs(bench_dir: Path, he_dir: Path, he_build_dir: Path, logs: List[Dict[str, str]]) -> None:
    src = bench_dir / "fhe_io_example.txt"
    if not src.exists():
        logs.append({"step": "stage_inputs", "stdout": "", "stderr": "fhe_io_example.txt missing"})
        return
    he_build_dir.mkdir(parents=True, exist_ok=True)
    dest = he_build_dir / "fhe_io_example.txt"
    shutil.copy(src, dest)
    logs.append({"step": "stage_inputs", "stdout": f"Copied {src} -> {dest}", "stderr": ""})


def _stage_he_sources(bench_dir: Path, benchmark: str, logs: List[Dict[str, str]]) -> Path:
    """
    The compiler emits _gen_he_fhe.cpp/main.cpp into build/benchmarks/<bench>/he.
    We need a CMakeLists.txt and utils.{cpp,hpp} co-located before configuring.
    Returns the path to the staged HE source directory.
    """
    generated_he = bench_dir / "he"
    cmake_src = REPO_ROOT / "benchmarks" / benchmark / "he" / "CMakeLists.txt"
    util_cpp = REPO_ROOT / "benchmarks" / "utils.cpp"
    util_hpp = REPO_ROOT / "benchmarks" / "utils.hpp"

    missing = [p for p in (generated_he, cmake_src, util_cpp, util_hpp) if not p.exists()]
    if missing:
        msg = f"Missing HE artifacts: {missing}"
        logs.append({"step": "stage_he_sources", "stdout": "", "stderr": msg})
        raise BenchmarkRunError(msg, logs=logs.copy())

    # Copy CMakeLists and utils into the generated folder so CMake sees all sources together.
    shutil.copy(cmake_src, generated_he / "CMakeLists.txt")
    shutil.copy(util_cpp, generated_he / "utils.cpp")
    shutil.copy(util_hpp, generated_he / "utils.hpp")
    logs.append(
        {
            "step": "stage_he_sources",
            "stdout": f"Staged CMakeLists and utils into {generated_he}",
            "stderr": "",
        }
    )
    return generated_he


def _reset_he_build(he_build: Path, logs: List[Dict[str, str]]) -> None:
    if he_build.exists():
        shutil.rmtree(he_build, ignore_errors=True)
    logs.append({"step": "reset_he_build", "stdout": f"Cleaned {he_build}", "stderr": ""})


def _system_compiler_env() -> Dict[str, str]:
    env = os.environ.copy()
    env["CC"] = "/usr/bin/gcc"
    env["CXX"] = "/usr/bin/g++"
    return env


def _validate_polynomial_params(params: RunParams) -> None:
    options = list_polynomial_options()
    combinations = options.get("combinations", [])
    if combinations:
        match = any(
            c["depth"] == params.depth
            and c["iteration"] == params.iteration
            and c["regime"] == params.regime
            for c in combinations
        )
        if not match:
            combos = ", ".join(
                f"{c['regime']}-d{c['depth']}-i{c['iteration']}" for c in combinations
            )
            raise BenchmarkRunError(
                f"Invalid polynomial parameters depth={params.depth}, iteration={params.iteration}, regime={params.regime}. "
                f"Available combinations: {combos or 'none discovered'}."
            )


def _ensure_polynomial_inputs(bench_dir: Path, logs: List[Dict[str, str]]) -> None:
    """
    The polynomial HE CMakeLists expects fhe_io_example_adapted.txt one level above the he folder.
    Create it from fhe_io_example.txt if needed so builds do not fail.
    """
    base = bench_dir / "fhe_io_example.txt"
    adapted = bench_dir / "fhe_io_example_adapted.txt"
    if adapted.exists():
        logs.append({"step": "stage_polynomial_inputs", "stdout": f"Found {adapted}", "stderr": ""})
        return
    if base.exists():
        shutil.copy(base, adapted)
        logs.append(
            {"step": "stage_polynomial_inputs", "stdout": f"Copied {base} -> {adapted}", "stderr": ""}
        )
    else:
        logs.append(
            {
                "step": "stage_polynomial_inputs",
                "stdout": "",
                "stderr": "No fhe_io_example.txt to adapt for polynomial run",
            }
        )


def run_benchmark(params: RunParams) -> RunResult:
    logs: List[Dict[str, str]] = []
    benchmark = params.benchmark
    if benchmark not in DEFAULT_BENCHMARKS:
        raise BenchmarkRunError(f"Unsupported benchmark '{benchmark}'.", logs=logs.copy())
    if benchmark == "polynomials_coyote":
        _validate_polynomial_params(params)

    _ensure_configured(logs)
    _build_benchmark(benchmark, logs)

    bench_dir = BUILD_DIR / "benchmarks" / benchmark
    if not bench_dir.exists():
        raise BenchmarkRunError(
            f"Build folder for {benchmark} not found at {bench_dir}.", logs=logs.copy()
        )

    _maybe_generate_inputs(bench_dir, benchmark, params.slot_count, logs)
    _require_file(bench_dir / "fhe_io_example.txt", "check inputs", logs)

    if benchmark == "polynomials_coyote":
        command = [
            f"./{benchmark}",
            str(params.depth),
            str(params.iteration),
            str(params.regime),
            str(params.vectorize_code),
            str(params.optimization_method),
        ]
    else:
        command = [
            f"./{benchmark}",
            str(params.vectorize_code),
            str(params.slot_count),
            str(params.optimization_method),
            str(params.window),
            str(params.call_quantifier),
            str(params.cse),
            str(params.const_folding),
        ]
    logs.append(
        {
            "step": "compile_cmd",
            "stdout": f"cd {bench_dir} && " + " ".join(command),
            "stderr": "",
        }
    )
    # Debugging: Check files in bench_dir
    fhe_io = bench_dir / "fhe_io_example.txt"
    if fhe_io.exists():
        logs.append({"step": "debug", "stdout": f"Found {fhe_io}", "stderr": ""})
    else:
        files = list(bench_dir.glob("*"))
        logs.append({"step": "debug", "stdout": f"Missing {fhe_io}. Files: {files}", "stderr": ""})

    stdout, stderr = _run_logged("compile", command, bench_dir, logs, timeout=1800)
    compile_ms, depth, multiplicative_depth = _parse_compile_metrics(stdout)
    if benchmark == "polynomials_coyote":
        _ensure_polynomial_inputs(bench_dir, logs)

    initial_expr, optimized_expr, vector_sizes = _read_expr_files(bench_dir)

    he_src = _stage_he_sources(bench_dir, benchmark, logs)
    he_build = bench_dir / "he_build"
    _reset_he_build(he_build, logs)
    _stage_he_inputs(bench_dir, he_src, he_build, logs)
    _require_file(he_build / "fhe_io_example.txt", "check he input", logs)

    cmake_env = _system_compiler_env()
    _run_logged("cmake_he", ["cmake", "-S", str(he_src), "-B", str(he_build)], he_src, logs, env=cmake_env)
    _run_logged("build_he", ["cmake", "--build", str(he_build)], he_src, logs, env=cmake_env)
    exe_stdout, exe_stderr = _run_logged("he_run", ["./main"], he_build, logs, timeout=900, env=cmake_env)
    execution_ms, remaining_noise = _parse_execution_metrics(exe_stdout)

    return RunResult(
        benchmark=benchmark,
        slot_count=params.slot_count,
        optimization_method=params.optimization_method,
        compile_ms=compile_ms,
        depth=depth,
        multiplicative_depth=multiplicative_depth,
        execution_ms=execution_ms,
        remaining_noise_budget=remaining_noise,
        initial_expr=initial_expr,
        optimized_expr=optimized_expr,
        vector_sizes=vector_sizes,
        logs=logs,
    )


def list_benchmarks() -> List[str]:
    candidates = []
    for bench in DEFAULT_BENCHMARKS:
        if (REPO_ROOT / "benchmarks" / bench).exists():
            candidates.append(bench)
    return candidates


def run_and_serialize(params: Dict) -> str:
    run_params = RunParams(**params)
    result = run_benchmark(run_params)
    return json.dumps(result.to_json(), indent=2)
