from stable_baselines3 import PPO
import time
from .utils import load_expressions, create_rules, parse_sexpr, load_embedding_model,predict_method,calc_vec_sizes
import sys
from .env import fheEnv
from stable_baselines3.common.vec_env import DummyVecEnv, VecNormalize,SubprocVecEnv
from .policy import HierarchicalMaskablePolicy
import sys, importlib

from stable_baselines3.common.monitor import Monitor
def run_agent(expressions_file: str,embeddings_model, model_filepath: str,output_file: str):
    start_time = time.perf_counter()
    expressions = load_expressions(expressions_file)
    if not len(expressions):
        print("No valid expressions found in the file.")
        sys.exit(1)
        return
    rules_list = create_rules("rules.txt")
    rules_list["END"] = None
    results = []
    max_positions = 16
    end_time = time.perf_counter()
    elapsed_seconds = end_time - start_time
    env = DummyVecEnv([
    lambda: Monitor(fheEnv(rules_list, expressions, max_positions=max_positions,embeddings_model=embeddings_model))
    ])
    model = PPO(
        policy=HierarchicalMaskablePolicy,
        env=env
    )
    sys.modules["fhe_rl_new"] = importlib.import_module("fhe_rl")
    model = model.load(model_filepath)
    start_time = time.perf_counter()
    obs = env.reset()
    wrapper  = env.envs[0]
    fhe_env   = wrapper.env
    test_expr = fhe_env.initial_expression
    initial_cost = fhe_env.initial_cost
    done = False
    steps = 0
    last_expr = None
    bad_count = 0
    while not done:
        last_expr = fhe_env.expression
        last_cost = fhe_env.current_cost
        action, _ = model.predict(obs, deterministic=True)
        obs, rewards, dones, infos = env.step(action)
        done = bool(dones[0])
        steps += 1
    parsed = parse_sexpr(last_expr)
    vec_sizes=" ".join(str(x) for x in calc_vec_sizes(parsed))
    with open (output_file, "w") as f:
        f.write(last_expr+"\n"+vec_sizes)
    end_time = time.perf_counter()
    elapsed_seconds = end_time - start_time
    