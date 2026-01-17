
import sys
import os
import argparse
from .run import run_agent
from .train import train_agent
from .test import test_agent
from .utils import load_embeddings
from .TRAE_bpe import BPETokenizer  # Import for pickle compatibility
from .config import (
    get_model_path, get_tokenizer_type, 
    print_config
)


def parse_arguments(args=None):
    """Parse command line arguments"""
    parser = argparse.ArgumentParser(description="FHE RL Agent")
    
    # Add tokenizer type argument
    parser.add_argument(
        '--tokenizer_type', 
        choices=['dynamic', 'bpe'], 
        default=get_tokenizer_type(),
        help='Tokenizer type to use (default: from config)'
    )
    
    # Add config flag
    parser.add_argument(
        '--show_config', 
        action='store_true',
        help='Show current configuration and exit'
    )
    
    # Subcommands
    subparsers = parser.add_subparsers(dest='mode', help='Available commands')
    
    # Train command
    train_parser = subparsers.add_parser('train', help='Train the agent')
    
    # Test command
    test_parser = subparsers.add_parser('test', help='Test the agent')
    
    # Run command
    run_parser = subparsers.add_parser('run', help='Run the agent')
    run_parser.add_argument('input_expr_file', help='Input expression file')
    run_parser.add_argument('output_vector_file', help='Output vector file')
    
    return parser.parse_args(args)


def usage() -> None:
    print(
        "Usage:\n"
        "  python -m fhe_rl train [--tokenizer_type {dynamic,bpe}]\n"
        "  python -m fhe_rl test  [--tokenizer_type {dynamic,bpe}]\n"
        "  python -m fhe_rl run   [--tokenizer_type {dynamic,bpe}] "
        "<input_expr_file> <output_vector_file>\n"
        "  python -m fhe_rl --show_config  # Show current configuration\n"
        "\n"
        "Options:\n"
        "  --tokenizer_type {dynamic,bpe}  Choose tokenizer type (overrides config)\n"
        "  --show_config                   Show current configuration\n"
        "\n"
        "All model paths are loaded from config.py."
    )
    sys.exit(1)


def load_embeddings_from_config(tokenizer_type=None):
    """Load embeddings using the configuration system"""
    try:
        # Determine the correct embeddings model based on tokenizer type
        if tokenizer_type == "bpe" or (tokenizer_type is None and get_tokenizer_type() == "bpe"):
            embeddings_path = get_model_path("bpe_embeddings_model")
        else:
            embeddings_path = get_model_path("dynamic_embeddings_model")
        
        return load_embeddings(tokenizer_type=tokenizer_type, checkpoint_path=embeddings_path)
    except FileNotFoundError as e:
        print(f"Error: {e}")
        sys.exit(1)


def main(args=None):
    """Main function with configuration support"""
    parsed_args = parse_arguments(args)
    
    # Show configuration if requested
    if parsed_args.show_config:
        print_config()
        return
    
    mode = parsed_args.mode
    if not mode:
        usage()

    # ────────────────────────────── TRAIN ─────────────────────────────
    if mode == "train":
        embeddings, tokenizer = load_embeddings_from_config(parsed_args.tokenizer_type)
        train_agent("./fhe_rl/datasets/new_dataset_random.txt", embeddings, total_timesteps=2_000_000)

    # ─────────────────────────────── TEST ─────────────────────────────
    elif mode == "test":
        agent_zip = get_model_path("agent_model")
        embeddings, tokenizer = load_embeddings_from_config(parsed_args.tokenizer_type)
        test_agent("./fhe_rl/datasets/benchmarks.txt", embeddings, agent_zip)

    # ─────────────────────────────── RUN ──────────────────────────────
    elif mode == "run":
        agent_zip = get_model_path("agent_model")
        input_file = parsed_args.input_expr_file
        output_file = parsed_args.output_vector_file
        embeddings, tokenizer = load_embeddings_from_config(parsed_args.tokenizer_type)
        run_agent(input_file, embeddings, agent_zip, output_file)

    else:
        print("Invalid command. Use 'train', 'test' or 'run'.")
        usage()




if __name__ == "__main__":
    main()