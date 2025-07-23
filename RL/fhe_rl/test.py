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
    max_positions = 32
    env = DummyVecEnv([
    lambda: Monitor(fheEnv(rules_list, expressions, max_positions=max_positions,embeddings_model=embeddings_model))
    ])
    test_env = VecNormalize(
    env,
    norm_obs=False,
    norm_reward=True,
    clip_reward=100.0 
    )
    model = PPO(
        policy=HierarchicalMaskablePolicy,
        env=test_env,
        policy_kwargs={
            "ent_coef": 0.1,
            "rule_dim":      len(rules_list),
            "max_positions": max_positions,
            "rule_hidden_dims":   [128, 128],
            "pos_hidden_dims":    [128, 128],
            "value_hidden_dims":    [256, 128, 64],
        },
    )
    model = model.load(model_filepath)
    def predict_method(
        self,
        observation,
        state=None,
        episode_start=None,
        deterministic: bool = False,
    ):
        device = next(self.parameters()).device
        if isinstance(observation, dict):
            obs = {k: torch.as_tensor(v, device=device) for k, v in observation.items()}
        else:
            obs = torch.as_tensor(observation, device=device)
        actions, _, _ = self.forward(obs, deterministic=deterministic)
        return actions.cpu().numpy(), state
    import types
    model.policy.predict = types.MethodType(predict_method, model.policy)
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
        last_cost = None
        final_expr = None

        while not done:
            last_expr = fhe_env.expression
            last_cost = fhe_env.current_cost

            action, _ = model.predict(obs, deterministic=True)
            obs, rewards, dones, infos = env.step(action)
            if rewards[0] < 0 and not final_expr:
                final_expr = last_expr
            if rewards[0] > 0 and final_expr:
                final_expr = None
            if rewards[0] < 0 and final_expr:
                final_expr = last_expr
                break
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

