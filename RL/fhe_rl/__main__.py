
import sys
import os
from .run   import run_agent
from .train import train_agent
from .test  import test_agent
from .utils import load_embedding_model


def usage() -> None:
    print(
        "Usage:\n"
        "  python -m fhe_rl.main train <embeddings_model>\n"
        "  python -m fhe_rl.main test  <agent_zip> <embeddings_model>\n"
        "  python -m fhe_rl.main run   <agent_zip> <embeddings_model> "
        "<input_expr_file> <output_vector_file>"
    )
    sys.exit(1)


def load_embeddings(path: str):
    if not os.path.isfile(path):
        print(f"Error: embeddings model '{path}' not found.")
        sys.exit(1)
    return load_embedding_model(checkpoint_path=path, device="cuda")



if __name__ == "__main__":

    if len(sys.argv) < 2:
        usage()

    mode = sys.argv[1].lower()

    # ────────────────────────────── TRAIN ─────────────────────────────
    if mode == "train":
        if len(sys.argv) < 3:
            usage()
        embeddings_path = sys.argv[2]
        embeddings = load_embeddings(embeddings_path)
        train_agent(".dataset/all_expressions_cleaned.txt", embeddings, total_timesteps=1_000_000)

    # ─────────────────────────────── TEST ─────────────────────────────
    elif mode == "test":
        if len(sys.argv) < 4:
            usage()
        agent_zip       = sys.argv[2]
        embeddings_path = sys.argv[3]
        embeddings = load_embeddings(embeddings_path)
        test_agent(".dataset/test.txt", embeddings, agent_zip)

    # ─────────────────────────────── RUN ──────────────────────────────
    elif mode == "run":
        if len(sys.argv) < 6:
            usage()
        agent_zip         = sys.argv[2]
        embeddings_path   = sys.argv[3]
        input_file        = sys.argv[4]
        output_file       = sys.argv[5]
        embeddings = load_embeddings(embeddings_path)
        run_agent(input_file, embeddings, agent_zip, output_file)

    # ────────────────────────── BAD SUBCOMMAND ────────────────────────
    else:
        print("Invalid command. Use 'train', 'test' or 'run'.")
        usage()