import os, sys
from pytrs import create_rules as _create_rules,parse_sexpr,Expr,Const,Var,Op

import torch, torch.nn as nn
from .TRAE import TRAE,get_expression_cls_embedding,flatten_expr

def create_rules(path):
    return _create_rules(path=path)

def load_embedding_model(checkpoint_path=None, device="cpu"):
    embeddings_model = TRAE()  
    state_dict = torch.load(checkpoint_path, map_location=device,weights_only=True)
    new_sd = {k[len("module.") :] if k.startswith("module.") else k: v for k, v in state_dict.items()}
    embeddings_model.load_state_dict(new_sd)
    embeddings_model.to(device) 
    embeddings_model.eval()
    return embeddings_model

def get_token_sequence(exp_str: str):
    expr = parse_sexpr(exp_str)
    flat = flatten_expr(expr)
    node_ids = tuple(entry["node_id"] for entry in flat)
    return node_ids

def load_expressions(file_path: str,validation_exprs = []):
    validation_token_set = set()
    for val in validation_exprs:
        exp_str = val.strip()
        
        token_seq = get_token_sequence(exp_str)
        validation_token_set.add(token_seq)
        
    
    unique_expressions = {}
    recap = { "1": 0,"2":0, "4": 0, "8": 0, "16": 0, "32": 0,"25":0 ,"9":0}

    with open(file_path, "r") as f:
        for line in f:
            exp_str = line.split("|||")[0].strip()
            if not exp_str:
                continue
            try:
                
                expr = parse_sexpr(exp_str)
            except Exception as e:
                continue
            vec_size = len(expr.args)
            if  not (vec_size > 0 and (vec_size & (vec_size - 1) == 0) and vec_size <= 32):
                continue
            token_seq = get_token_sequence(exp_str)
            if token_seq in validation_token_set:
                continue

            if token_seq not in unique_expressions:
                recap[str(vec_size)] += 1
                unique_expressions[token_seq] = exp_str
            
    print(recap)
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
        if residual and prev == h and i % 2 == 1:
            layers.append(Residual())
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