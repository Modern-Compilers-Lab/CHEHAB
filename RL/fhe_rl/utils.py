import os
import sys
from pytrs import (
    create_rules as _create_rules, parse_sexpr, tokenize,
    Expr, Const, Var, Op, VARIABLE_RANGE, CONST_OFFSET, 
    PAREN_CLOSE, PAREN_OPEN, node_to_id, get_normal_depth
)
import torch
import torch.nn as nn

from .config import (
    get_model_path, get_tokenizer_type, get_device, get_vocab_size,
    TOKENIZER_CONFIG, AGENT_CONFIG
)

if get_tokenizer_type() == "bpe":
    from .TRAE_bpe import TRAE, get_expression_cls_embedding, BPETokenizer
else:   
    from .TRAE import TRAE, get_expression_cls_embedding

DEVICE = get_device()

def load_embeddings(tokenizer_type=None, checkpoint_path=None, device=None):

    if tokenizer_type is None:
        tokenizer_type = get_tokenizer_type()
    
    if device is None:
        device = get_device()
    
    if checkpoint_path is None:
        checkpoint_path = get_model_path("embeddings_model")
    
    print(f"Loading embeddings with tokenizer type: {tokenizer_type}")
    
    if tokenizer_type == "bpe":
        return load_embedding_model_bpe(checkpoint_path, device)
    else:
        model = load_embedding_model_dynamic(checkpoint_path, device)
        return model, None  
def create_rules(path):
    return _create_rules(path=path)

def load_embedding_model_dynamic(checkpoint_path=None, device=DEVICE):
    embeddings_model = TRAE()  
    state_dict = torch.load(checkpoint_path, map_location=device,weights_only=True)
    new_sd = {k[len("module.") :] if k.startswith("module.") else k: v for k, v in state_dict.items()}
    embeddings_model.load_state_dict(new_sd)
    embeddings_model.to(device) 
    embeddings_model.eval()
    return embeddings_model

def load_embedding_model_bpe(checkpoint_path=None, device="cpu"):
    # Load state dict first to extract vocab size
    state_dict = torch.load(checkpoint_path, map_location=device, weights_only=True)
    new_sd = {k[len("module.") :] if k.startswith("module.") else k: v for k, v in state_dict.items()}
    
    # Extract vocab size from the model state dict
    if 'model.token_embedding.weight' in new_sd:
        vocab_size = new_sd['model.token_embedding.weight'].shape[0]
    elif 'token_embedding.weight' in new_sd:
        vocab_size = new_sd['token_embedding.weight'].shape[0]
    else:
        print("Warning: Could not determine vocab size from state dict, using default 1000")
        vocab_size = 1000
    
    print(f"Detected model vocab size: {vocab_size}")
    
    # Load BPE tokenizer if available
    try:
        import pickle
        
        tokenizer_paths = [
            "./fhe_rl/trained_models/bpe_tokenizer.pkl"
        ]
        
        loaded_tokenizer = None
        for path in tokenizer_paths:
            try:
                with open(path, "rb") as f:
                    loaded_tokenizer = pickle.load(f)
                print(f"BPE tokenizer loaded from: {path}")
                break
            except (FileNotFoundError, OSError) as e:
                print(e)
                continue
        
        if loaded_tokenizer is None:
            raise FileNotFoundError("BPE tokenizer not found in any expected location")
        
        # Update the global config and tokenizer in TRAE module
        from . import TRAE_bpe as   TRAE_module
        print(vocab_size)
        TRAE_module.config.vocab_size = vocab_size
        TRAE_module.tokenizer = loaded_tokenizer
        loaded_tokenizer.trained = True
        TRAE_module.loaded_tokenizer = loaded_tokenizer
        print("BPE tokenizer loaded for embeddings.")
    except (FileNotFoundError, AttributeError, pickle.PickleError) as e:
        print(f"Warning: BPE tokenizer loading failed: {e}")
        print("Using default tokenizer setup.")
        # Update the global config in TRAE module
        from . import TRAE_bpe  as  TRAE_module
        TRAE_module.config.vocab_size = vocab_size
    
    # Now create model with correct vocab size
    embeddings_model = TRAE()  
    embeddings_model.load_state_dict(new_sd)
    embeddings_model.to(device) 
    embeddings_model.eval()
    return embeddings_model,None


def get_token_sequence(exp_str: str):
    expr = parse_sexpr(exp_str)
    flat = flatten_expr(expr)
    node_ids = tuple(entry["node_id"] for entry in flat)
    return node_ids
def dfs_traverse(expr, depth=0, node_list=None):
    if node_list is None:
        node_list = []
    if isinstance(expr, Op):
        node_list.append((PAREN_OPEN, depth))
        node_list.append((expr, depth))
        for child in expr.args:
            dfs_traverse(child, depth + 1, node_list)
        node_list.append((PAREN_CLOSE, depth))
    else:
        node_list.append((expr, depth))
    return node_list


def flatten_expr(expr):
    node_list = dfs_traverse(expr, 0)
    results = []
    varmap = {}
    intmap = {}
    next_var_id = VARIABLE_RANGE[0]
    next_int_id = CONST_OFFSET
    for node_or_paren, depth in node_list:
        if node_or_paren in (PAREN_OPEN, PAREN_CLOSE):
            nid = node_or_paren
        else:
            nid, next_var_id, next_int_id, _ = node_to_id(
                node_or_paren, varmap, intmap, next_var_id, next_int_id
            )
        results.append({"node_id": nid})
    return results

def load_expressions(file_path: str,validation_exprs = []):
    validation_token_set = set()
    for val in validation_exprs:
        exp_str = val.strip()
        
        token_seq = get_token_sequence(exp_str)
        validation_token_set.add(token_seq)
    unique_expressions = {}
    recap = { "1":0,"4":0,"8":0,"9":0,"16":0,"25":0,"32":0}
    with open(file_path, "r") as f:
        for line in f:
            exp_str = line.split(":")[0].strip()
            if not exp_str:
                continue
            try:
                
                expr = parse_sexpr(exp_str)
                vec_size = len(expr.args)
                if  not (str(vec_size) in recap.keys()):
                    continue
                token_seq = get_token_sequence(exp_str)
                if token_seq in validation_token_set:
                    continue

                if token_seq not in unique_expressions:
                    recap[str(vec_size)] += 1
                    unique_expressions[token_seq] = exp_str
            except Exception as e:
                print(e)
                continue
    print("Number of unique valid expressions (excluding validation):", len(unique_expressions))
    return list(unique_expressions.values())


def mlp(in_dim, hidden_dims, out_dim, *,
        act=nn.GELU, layernorm=True, dropout=0.0,
        residual=False):
    """
    Build an MLP: [in_dim] → hidden_dims* → [out_dim]

    Args
    ----
    hidden_dims : list[int]  (e.g. [1024, 1024, 512])
    residual    : if True, adds skip-connections every two layers
    """
    layers, prev = [], in_dim
    for i, h in enumerate(hidden_dims):
        layers.append(nn.Linear(prev, h))
        if layernorm:
            layers.append(nn.LayerNorm(h))
        layers.append(act())
        if dropout > 0:
            layers.append(nn.Dropout(dropout))
        # Note: Residual connections removed for simplicity
        # if residual and prev == h and i % 2 == 1:
        #     layers.append(Residual())
        prev = h
    layers.append(nn.Linear(prev, out_dim))
    return nn.Sequential(*layers)


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

def calc_vec_sizes(expr:Expr):
    vec_sizes=[]
    def rec(node:Expr):
        if isinstance(node, (Const, Var)):
            return
        if isinstance(node, Op):
            if node.op == "Vec":
                vec_sizes.append(len(node.args))
            else:
                for arg in node.args:
                    rec(arg)
            
    rec(expr)
    return vec_sizes