import os
from stable_baselines3.common.vec_env import SubprocVecEnv, VecNormalize
from stable_baselines3.common.monitor import Monitor
from .env    import fheEnv
from .policy import HierarchicalMaskablePolicy
from stable_baselines3 import PPO
from .utils  import load_expressions, create_rules, load_embedding_model
from .logger import log_training_details
from stable_baselines3.common.vec_env import SubprocVecEnv, VecNormalize,DummyVecEnv
from .callbacks import linear_schedule, EntCoefScheduler
from stable_baselines3.common.callbacks import EvalCallback

def train_agent(expressions_file: str, embeddings_model, total_timesteps: int = 1_000_000, num_envs: int = 8):
    benchmarks = load_expressions("./fhe_rl/datasets/benchmarks.txt") 
    expressions = load_expressions(expressions_file, benchmarks)
    max_positions = 16
    rules_list  = create_rules("rules.txt")
    rules_list["END"] = None
    job_id = os.environ.get("SLURM_JOB_ID", "jobid")
    run_name = f"model_{job_id}"
    tensorboard_log_dir = f"./tensorboard/{run_name}"
    def make_env(): return Monitor(fheEnv(rules_list, expressions, max_positions=max_positions, embeddings_model=embeddings_model))
    env = SubprocVecEnv([make_env for _ in range(num_envs)], start_method='spawn')    
    val_env = DummyVecEnv([
    lambda: Monitor(fheEnv(rules_list, benchmarks, max_positions=max_positions,embeddings_model=embeddings_model))
    ])
    ent_schedule = linear_schedule(0.1)
    model_params = {
        "policy": HierarchicalMaskablePolicy,
        "env": env,
        "learning_rate": 1e-4,
        "n_steps": 2048,
        "batch_size": 256,
        "gamma": 0.99,
        "gae_lambda": 0.98,
        "n_epochs": 15,
        "clip_range": 0.1,
        "clip_range_vf": 0.2,
        "ent_coef": 0.1,
        "verbose": 1,
        "tensorboard_log": tensorboard_log_dir,
        "policy_kwargs": {
            "ent_coef": 0.1,
            "rule_dim":      len(rules_list),
            "max_positions": max_positions,
            "rule_hidden_dims":   [128, 64],
            "pos_hidden_dims":    [64, 64],
            "value_hidden_dims":    [256, 128, 64],
        }
    }
    model = PPO(**model_params)
    log_training_details(
        model_params,
        job_id,
        num_data=len(expressions),
        num_actions=len(rules_list),
        total_timesteps=total_timesteps,
        output_model_name=run_name,
        notes="2 level hierarchical PPO max steps 75 and 8 envs"
    )
    num_benchmarks = len(benchmarks)
    eval_callback = EvalCallback(
        val_env, 
        best_model_save_path=f"./eval/best_model_{run_name}", 
        log_path=tensorboard_log_dir, 
        eval_freq=10000,
        n_eval_episodes=num_benchmarks,
        deterministic=True, 
        render=False, 
        verbose=1
    )
    model.learn(
        total_timesteps=total_timesteps, 
        log_interval=1, 
        progress_bar=True, 
        callback=[eval_callback,EntCoefScheduler(ent_schedule)]
    )
    model.save(run_name)