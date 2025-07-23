import argparse
from tqdm import tqdm
from .config import GeneratorConfig
from .chain_builder import build_chain
from .post_processor import remove_duplication
def parse_args() -> GeneratorConfig:
    p = argparse.ArgumentParser()
    p.add_argument("-v", "--vec-size",  type=int, required=True)
    p.add_argument("-n", "--iterations", type=int, default=100)
    p.add_argument("-o", "--output", type=str, default="vec_lang_expressions.txt")
    args = p.parse_args()
    return GeneratorConfig(vec_size=args.vec_size, iterations=args.iterations, output_path=args.output)

def main():
    cfg = parse_args()
    chain = build_chain(cfg)
    with open(cfg.output_path, "a") as out:
        for _ in tqdm(range(cfg.iterations), desc=f"VecSize={cfg.vec_size}"):
            resp = chain.invoke({"vec_size": cfg.vec_size})
            out.write(resp + "\n")
            
    unique_expressions = remove_duplication(cfg.output_path)
    with open(cfg.output_path, "w") as out:
        for expr in unique_expressions:
            out.write(expr + "\n")

if __name__ == "__main__":
    main()
