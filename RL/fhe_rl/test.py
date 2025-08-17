from .logger import log_test_results
from stable_baselines3 import PPO
import sys
from .utils import load_expressions, load_embedding_model,create_rules
from .env import fheEnv
from .policy import HierarchicalMaskablePolicy
from stable_baselines3.common.vec_env import DummyVecEnv, VecNormalize
from stable_baselines3.common.monitor import Monitor
import os
import torch
def test_agent(expressions_file: str,embeddings_model, model_filepath: str):
    expressions = load_expressions(expressions_file)
    rules_list = create_rules("rules.txt")
    rules_list["END"] = None
    results = []
    max_positions = 16
    env = DummyVecEnv([
    lambda: Monitor(fheEnv(rules_list, expressions, max_positions=max_positions,embeddings_model=embeddings_model))
    ])
    model = PPO(
        policy=HierarchicalMaskablePolicy,
        env=test_env,
    )
    model = model.load(model_filepath)
    num_expr = len(expressions)
    for _ in range(num_expr):
        obs = env.reset()
        wrapper  = env.envs[0]
        fhe_env   = wrapper.env
        test_expr = fhe_env.initial_expression
        initial_cost = fhe_env.initial_cost
        done = False
        steps = 0
        last_expr = None
        while not done:
            last_expr = fhe_env.expression
            last_cost = fhe_env.current_cost
            done = bool(dones[0])
            steps += 1
        results.append({
            "Test Expression":   test_expr,
            "Final Expression":  final_expr,
            "Initial Cost":      initial_cost,
            "Final Cost":        last_cost,
            "Steps":             steps
        })
    
        print({
            "Test Expression":   test_expr,
            "Final Expression":  final_expr,
            "Initial Cost":      initial_cost,
            "Final Cost":        last_cost,
            "Steps":             steps
        })
    job_id = os.environ.get("SLURM_JOB_ID", "jobid")
    sheet_name = f"HierarchicalPPO_Test_{job_id}"
    log_test_results(results, sheet_name=sheet_name)
    return results

