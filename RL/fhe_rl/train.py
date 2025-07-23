import os
from stable_baselines3.common.vec_env import SubprocVecEnv, VecNormalize
from stable_baselines3.common.monitor import Monitor
from .env    import fheEnv
from .policy import HierarchicalMaskablePolicy
from stable_baselines3 import PPO
from .utils  import load_expressions, create_rules,load_embedding_model
from .logger import log_training_details
from stable_baselines3.common.vec_env import SubprocVecEnv, VecNormalize
from .callbacks import linear_schedule,EntCoefScheduler

def train_agent(expressions_file: str,embeddings_model, total_timesteps: int = 1_000_000, num_envs: int = 1):
    expressions = load_expressions(expressions_file)
    max_positions = 32
    rules_list  = create_rules("rules.txt")
    rules_list["END"] = None
    job_id = os.environ.get("SLURM_JOB_ID", "jobid")
    run_name = f"model_{job_id}"
    tensorboard_log_dir = f"./tensorboard/{run_name}"
    def make_env(): return Monitor(fheEnv(rules_list, expressions, max_positions=32,embeddings_model=embeddings_model))
    env = SubprocVecEnv([make_env for _ in range(num_envs)])
    train_env = VecNormalize(env, norm_obs=False, norm_reward=True, clip_reward=100.0)
    ent_schedule = linear_schedule(0.1)
    model_params = {
    "policy": HierarchicalMaskablePolicy,
    "env": train_env,
    "learning_rate": 1e-4,
    "n_steps": 512,
    "batch_size": 128,
    "gamma": 0.99,
    "gae_lambda": 0.95,
    "n_epochs": 15,
    "clip_range": 0.1,
    "clip_range_vf": 0.1,
    "ent_coef": 0.1,
    "verbose": 1,
    "tensorboard_log": tensorboard_log_dir,
    "policy_kwargs": {
        "ent_coef":0.1,
        "rule_dim":      len(rules_list),
        "max_positions": max_positions,
        "rule_hidden_dims":   [128, 128],
        "pos_hidden_dims":    [128, 128],
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
        notes="2 level hierarchical PPO max steps 60 and 8 envs"
    )
    model.learn(total_timesteps=total_timesteps, log_interval=1, progress_bar=True,callback=[EntCoefScheduler(ent_schedule)])
    model.save(run_name)