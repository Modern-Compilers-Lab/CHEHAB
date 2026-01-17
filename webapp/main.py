import json
import subprocess
from pathlib import Path

from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import FileResponse
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel, Field, validator

from .benchmark_runner import (
    BenchmarkRunError,
    RunParams,
    list_benchmarks,
    list_polynomial_options,
    run_benchmark,
)

POLY_OPTIONS = list_polynomial_options()
POLY_DEFAULT = POLY_OPTIONS.get("default", {"depth": 5, "iteration": 1, "regime": "50-50"})


class RunRequest(BaseModel):
    benchmark: str = Field(..., description="Benchmark name (matches folder under benchmarks/)")
    slot_count: int = Field(..., ge=1)
    optimization_method: int = Field(1, ge=1, le=1)  # RL only
    vectorize_code: int = Field(1, ge=0, le=1)
    window: int = Field(0, ge=0)
    call_quantifier: int = Field(1, ge=0, le=1)
    cse: int = Field(1, ge=0, le=1)
    const_folding: int = Field(1, ge=0, le=1)
    # Polynomials-specific
    depth: int = Field(POLY_DEFAULT.get("depth", 5), ge=1)
    iteration: int = Field(POLY_DEFAULT.get("iteration", 1), ge=1)
    regime: str = Field(POLY_DEFAULT.get("regime", "50-50"))

    @validator("slot_count")
    def _slot_count_allowed(cls, v):
        from .benchmark_runner import ALLOWED_SLOT_COUNTS
        if v not in ALLOWED_SLOT_COUNTS:
            raise ValueError(f"slot_count must be one of {ALLOWED_SLOT_COUNTS}")
        return v

    @validator("vectorize_code", "call_quantifier", "cse", "const_folding")
    def _binary_flags(cls, v):
        if v not in (0, 1):
            raise ValueError("Flag fields must be 0 or 1")
        return v

    @validator("depth")
    def _depth_allowed(cls, v, values):
        if values.get("benchmark") == "polynomials_coyote":
            options = list_polynomial_options()
            allowed = options.get("depths", [])
            if allowed and v not in allowed:
                raise ValueError(f"depth must be one of {allowed}")
        return v

    @validator("iteration")
    def _iteration_allowed(cls, v, values):
        if values.get("benchmark") == "polynomials_coyote":
            options = list_polynomial_options()
            allowed = options.get("iterations", [])
            if allowed and v not in allowed:
                raise ValueError(f"iteration must be one of {allowed}")
        return v

    @validator("regime")
    def _regime_allowed(cls, v, values):
        options = list_polynomial_options()
        allowed = options.get("regimes") or {"50-50", "100-50", "100-100"}
        if values.get("benchmark") == "polynomials_coyote" and v not in allowed:
            raise ValueError(f"regime must be one of {sorted(allowed)}")
        return v


app = FastAPI(title="CHEHAB demo", version="0.1.0")
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

STATIC_DIR = Path(__file__).parent / "static"
app.mount("/static", StaticFiles(directory=STATIC_DIR), name="static")


@app.get("/api/benchmarks")
def get_benchmarks():
    poly_options = list_polynomial_options()
    poly_default = poly_options.get("default", POLY_DEFAULT)
    return {
        "benchmarks": list_benchmarks(),
        "defaults": {
            "slot_counts": [2, 4, 8, 16, 32],
            "window_options": [0, 1, 2, 4, 8],
            "flag_options": [0, 1],
            "optimization_methods": [
                {"value": 1, "label": "Reinforcement Learning"},
            ],
            "vectorize_code": 1,
            "window": 0,
            "call_quantifier": 1,
            "cse": 1,
            "const_folding": 1,
            "polynomials": {
                "depth": poly_default.get("depth"),
                "depth_options": poly_options.get("depths"),
                "iteration": poly_default.get("iteration"),
                "iteration_options": poly_options.get("iterations"),
                "regime": poly_default.get("regime"),
                "regimes": poly_options.get("regimes"),
                "combinations": poly_options.get("combinations"),
            },
        },
    }


@app.post("/api/run")
def run(req: RunRequest):
    try:
        result = run_benchmark(RunParams(**req.dict()))
    except subprocess.TimeoutExpired:
        raise HTTPException(status_code=504, detail="Benchmark run timed out.")
    except BenchmarkRunError as exc:
        detail = {"error": str(exc)}
        if getattr(exc, "logs", None):
            detail["logs"] = exc.logs
        raise HTTPException(status_code=400, detail=detail)
    except Exception as exc:
        raise HTTPException(status_code=500, detail=str(exc))
    return result.to_json()


@app.get("/", include_in_schema=False)
def index():
    return FileResponse(STATIC_DIR / "index.html")


if __name__ == "__main__":
    import uvicorn

    uvicorn.run("webapp.main:app", host="0.0.0.0", port=8000, reload=False)
