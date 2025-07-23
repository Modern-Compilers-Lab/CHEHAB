from stable_baselines3 import PPO
import time
from .utils import load_expressions, create_rules, parse_sexpr, load_embedding_model,predict_method,calc_vec_sizes
import sys
from .env import fheEnv
from stable_baselines3.common.vec_env import DummyVecEnv, VecNormalize
from .policy import HierarchicalMaskablePolicy

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
    max_positions = 32
    end_time = time.perf_counter()
    elapsed_seconds = end_time - start_time
    
    print(f"[run_agent] Elapsed time: {elapsed_seconds:.4f} seconds")
    #env = fheEnv(rules_list, expressions, max_positions=max_positions)
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
    
    model = model.load(model_filepath,device="cuda")
    
    import types
    model.policy.predict = types.MethodType(predict_method, model.policy)
    start_time = time.perf_counter()

    obs = env.reset()
    wrapper  = env.envs[0]
    fhe_env   = wrapper.env
    test_expr = fhe_env.initial_expression
    initial_cost = fhe_env.initial_cost

    done = False
    steps = 0

    last_expr = None
    final_expr = None
    bad_count = 0

    while not done:
        last_expr = fhe_env.expression
        last_cost = fhe_env.current_cost

        action, _ = model.predict(obs, deterministic=True)
        obs, rewards, dones, infos = env.step(action)

        if rewards[0] < 0:
            bad_count += 1
            if bad_count == 1:
                final_expr = last_expr
        else:
            bad_count = 0
            final_expr = None

        if bad_count >= 3:
            final_expr = last_expr
            break

        done = bool(dones[0])
        steps += 1
    if not final_expr:
        final_expr = last_expr
    parsed = parse_sexpr(final_expr)
    vec_sizes=" ".join(str(x) for x in calc_vec_sizes(parsed))
    with open (output_file, "w") as f:
        f.write(last_expr+"\n"+vec_sizes)
    end_time = time.perf_counter()
    elapsed_seconds = end_time - start_time
    print(f"[run_agent] Elapsed time: {elapsed_seconds:.4f} seconds")
    
    