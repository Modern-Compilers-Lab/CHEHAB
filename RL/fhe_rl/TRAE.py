import os
import builtins
import sys 

os.environ["PYTORCH_CUDA_ALLOC_CONF"] = "expandable_segments:True"

import torch
import torch.nn as nn
import torch.optim as optim
import torch.distributed as dist
from torch.nn.parallel import DistributedDataParallel as DDP
from torch.utils.data import DataLoader, DistributedSampler
import random
import sys
import time
import numpy as np

from pytrs import (
    Op,
    VARIABLE_RANGE,
    CONST_OFFSET,
    PAREN_CLOSE,
    PAREN_OPEN,
    node_to_id,
    parse_sexpr,
    tokenize,
    MAX_INT_TOKENS
)

torch.set_float32_matmul_precision("high")

# ------------------------------------------------------------------
# Helper: force-flushing print & memory reporter
# ------------------------------------------------------------------
def print(*args, **kwargs):  
    kwargs["flush"] = True
    builtins.print(*args, **kwargs)



# ------------------------------------------------------------------
# Subclass DDP to allow easier attribute access
# ------------------------------------------------------------------
class AEDDP(DDP):
    def __getattr__(self, name):
        try:
            return super().__getattr__(name)
        except AttributeError:
            return getattr(self.module, name)


# ------------------------------------------------------------------
# DDP setup and device assignment
# ------------------------------------------------------------------
ddp = 0 # int(os.environ.get("RANK", -1)) != -1
deviceids = [0, 1, 2]

if ddp:
    assert torch.cuda.is_available(), "DDP requires CUDA"
    dist.init_process_group(backend="nccl")
    ddp_rank = int(os.environ["RANK"])
    ddp_local_rank = int(os.environ["LOCAL_RANK"])
    ddp_world_size = int(os.environ["WORLD_SIZE"])
    device = f"cuda:{deviceids[ddp_rank]}"
    torch.cuda.set_device(device)
    master_process = ddp_rank == 0
else:
    master_process = True
    ddp_rank = 0
    ddp_world_size = 1
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")



def debug_print_memory(tag=""):  
    if master_process:
        if torch.cuda.is_available():
            alloc = torch.cuda.memory_allocated(device) / (1024**2)
            reserved = torch.cuda.memory_reserved(device) / (1024**2)
            print(f"[{tag}] CUDA  allocated={alloc:.2f} MiB  reserved={reserved:.2f} MiB")



if master_process:
    print("Using DDP with world size:", ddp_world_size)
    print("Assigned device:", device)
    
    
debug_print_memory("After device setup")  


# ------------------------------------------------------------------
# Configuration and token setup
# ------------------------------------------------------------------
class Config:
    max_gen_length = 25122

    vocab_size = CONST_OFFSET + MAX_INT_TOKENS + 2 + 1 + 1
    start_token = CONST_OFFSET + MAX_INT_TOKENS
    end_token = CONST_OFFSET + MAX_INT_TOKENS + 1
    pad_token = CONST_OFFSET + MAX_INT_TOKENS + 2

    cls_token = vocab_size
    vocab_size += 1  # include CLS

    d_model = 256
    num_heads = 8
    num_encoder_layers = 4
    num_decoder_layers = 4
    dim_feedforward = 512
    transformer_dropout = 0.2
    max_seq_length = 25200

    batch_size = 16 
    learning_rate = 3e-4
    epochs = 50
    dropout_rate = 0.3

    total_samples = 5000000  

config = Config()

# ------------------------------------------------------------------
# Expression processing helpers
# ------------------------------------------------------------------
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


# ------------------------------------------------------------------
# Positional encoding
# ------------------------------------------------------------------
class PositionalEncoding(nn.Module):
    def __init__(self, d_model, max_len=1024):
        super().__init__()
        self.pos_embedding = nn.Embedding(max_len, d_model)

    def forward(self, x, positions=None):
        batch_size, seq_len, _ = x.shape
        if positions is None:
            positions = (
                torch.arange(0, seq_len, device=x.device)
                .unsqueeze(0)
                .repeat(batch_size, 1)
            )
        pos_emb = self.pos_embedding(positions)
        return x + pos_emb


# ------------------------------------------------------------------
# Transformer autoencoder
# ------------------------------------------------------------------
class TransformerAutoencoder(nn.Module):
    def __init__(self):
        super().__init__()
        self.token_embedding = nn.Embedding(config.vocab_size, config.d_model)
        self.pos_encoder = PositionalEncoding(
            config.d_model, max_len=config.max_seq_length
        )

        enc_layer = nn.TransformerEncoderLayer(
            d_model=config.d_model,
            nhead=config.num_heads,
            dim_feedforward=config.dim_feedforward,
            dropout=config.transformer_dropout,
            batch_first=True,
        )
        self.encoder = nn.TransformerEncoder(enc_layer, config.num_encoder_layers)

        dec_layer = nn.TransformerDecoderLayer(
            d_model=config.d_model,
            nhead=config.num_heads,
            dim_feedforward=config.dim_feedforward,
            dropout=config.transformer_dropout,
            batch_first=True,
        )
        self.decoder = nn.TransformerDecoder(dec_layer, config.num_decoder_layers)

        self.output_fc = nn.Linear(config.d_model, config.vocab_size)

    @staticmethod
    def generate_square_subsequent_mask(sz, device):
        mask = (torch.triu(torch.ones(sz, sz, device=device)) == 1).T
        mask = mask.float().masked_fill(mask == 0, float("-inf")).masked_fill(
            mask == 1, float(0.0)
        )
        return mask

    def forward(self, src_nodes, tgt_seq):
        batch_size = src_nodes.size(0)
        cls_column = torch.full(
            (batch_size, 1), config.cls_token, dtype=torch.long, device=src_nodes.device
        )
        src_nodes_with_cls = torch.cat([cls_column, src_nodes], dim=1)

        src_emb = self.token_embedding(src_nodes_with_cls)
        src_emb = self.pos_encoder(src_emb)
        memory = self.encoder(src_emb)

        tgt_emb = self.token_embedding(tgt_seq)
        tgt_emb = self.pos_encoder(tgt_emb)
        tgt_mask = self.generate_square_subsequent_mask(
            tgt_seq.size(1), device=tgt_seq.device
        )

        dec_out = self.decoder(tgt_emb, memory, tgt_mask=tgt_mask)
        logits = self.output_fc(dec_out)
        return logits

    # ---------------- exposed helpers ----------------
    def encode(self, src_nodes):
        
        batch_size = src_nodes.size(0)
        cls_column = torch.full(
            (batch_size, 1), config.cls_token, dtype=torch.long, device=src_nodes.device
        )
        src_nodes_with_cls = torch.cat([cls_column, src_nodes], dim=1)
        src_emb = self.token_embedding(src_nodes_with_cls)
        src_emb = self.pos_encoder(src_emb)
        return self.encoder(src_emb)

    def decode_step(self, memory, partial_tgt_seq):
        tgt_emb = self.token_embedding(partial_tgt_seq)
        tgt_emb = self.pos_encoder(tgt_emb)
        mask = self.generate_square_subsequent_mask(
            partial_tgt_seq.size(1), partial_tgt_seq.device
        )
        dec = self.decoder(tgt_emb, memory, tgt_mask=mask)
        return self.output_fc(dec[:, -1, :])

    def get_cls_vector(self, memory):
        return memory[:, 0, :]


# ------------------------------------------------------------------
# TRAE wrapper
# ------------------------------------------------------------------
class TRAE(nn.Module):
    def __init__(self):
        super().__init__()
        self.model = TransformerAutoencoder()

    def forward(self, src_nodes, src_pos, tgt_seq):
        return self.model(src_nodes, tgt_seq)

    @property
    def encoder(self):
        return self.model.encode

    @property
    def decoder(self):
        return self.model

    def get_cls_summary(self, memory):
        return self.model.get_cls_vector(memory)


# ------------------------------------------------------------------
# Collate function
# ------------------------------------------------------------------
def collate_fn(batch):
    inputs, targets = [], []
    for expr in batch:
        flat = flatten_expr(expr)
        node_ids = [e["node_id"] for e in flat]
        tgt = [config.start_token] + node_ids + [config.end_token]
        inputs.append(node_ids)
        targets.append(tgt)

    max_src = max(len(x) for x in inputs)
    max_tgt = max(len(t) for t in targets)

    
    print(f"[collate] batch={len(batch)}  max_src={max_src}  max_tgt={max_tgt}")

    pad_src = [x + [config.pad_token] * (max_src - len(x)) for x in inputs]
    pad_tgt = [t + [config.pad_token] * (max_tgt - len(t)) for t in targets]

    return torch.tensor(pad_src, dtype=torch.long), torch.tensor(
        pad_tgt, dtype=torch.long
    )


# ------------------------------------------------------------------
# Training function
# ------------------------------------------------------------------
def train(model, train_dataset):
    accumulation_steps = 1

    train_sampler = None # DistributedSampler(train_dataset) if ddp else None
    train_loader = DataLoader(
        train_dataset,
        batch_size=config.batch_size,
        shuffle=train_sampler is None,
        sampler=train_sampler,
        collate_fn=collate_fn,
        num_workers=1,
        pin_memory=True,
    )

    opt = optim.AdamW(model.parameters(), lr=config.learning_rate)
    scheduler = optim.lr_scheduler.StepLR(opt, 1, gamma=0.5)
    loss_fn = nn.CrossEntropyLoss(ignore_index=config.pad_token)
    
    
    if master_process:
        print(f"[Rank {ddp_rank}] Begin training for {config.epochs} epochs")
        print(f"[Rank {ddp_rank}] Numer of batches: {len(train_loader)}")
    debug_print_memory("train start")  

    for epoch in range(config.epochs):
        epoch_start = time.time()
        model.train()
        if train_sampler:
            train_sampler.set_epoch(epoch)

        tot_loss, num_batches = 0.0, 0
        opt.zero_grad(set_to_none=True)
    
        for b_idx, (src_nodes, tgt_seq) in enumerate(train_loader):
            # -------------- diagnostics ----------------
            if master_process:

                print(f"[E{epoch+1} B{b_idx}] src={tuple(src_nodes.shape)} tgt={tuple(tgt_seq.shape)}")
                
            debug_print_memory(f"B{b_idx} loaded")  

            src_nodes = src_nodes.to(device)
            tgt_seq = tgt_seq.to(device)
            debug_print_memory(f"B{b_idx} moved")  

            try:
                logits = model(src_nodes, None, tgt_seq[:, :-1])
                debug_print_memory(f"B{b_idx} forward")  
            except RuntimeError as e:
                if "out of memory" in str(e):
                    print("*** OOM in forward ***")
                    debug_print_memory(f"OOM forward B{b_idx}")  
                    raise
                raise

            loss = loss_fn(logits.reshape(-1, config.vocab_size), tgt_seq[:, 1:].reshape(-1))
            (loss / accumulation_steps).backward()

            if (b_idx + 1) % accumulation_steps == 0:
                torch.nn.utils.clip_grad_norm_(model.parameters(), 1.0)
                opt.step()
                opt.zero_grad(set_to_none=True)
                debug_print_memory(f"B{b_idx} step")  

            tot_loss += loss.item()
            num_batches += 1

        dur = time.time() - epoch_start
        if ddp:
            t = torch.tensor(dur, device=device)
            dist.all_reduce(t, op=dist.ReduceOp.MAX)
            dur = t.item()

        if master_process:
            print(
                f"[Rank {ddp_rank}] Epoch {epoch+1} finished in {dur:.1f}s | "
                f"AvgLoss={tot_loss/num_batches:.4f}"
            )

        scheduler.step()
        if master_process:
            print(
                f"[Rank {ddp_rank}] LR after epoch {epoch+1}: "
                f"{opt.param_groups[0]['lr']:.6f}"
            )

        torch.cuda.empty_cache()
        debug_print_memory(f"epoch {epoch+1} end")  

        if master_process:
            job_id = os.environ.get("SLURM_JOB_ID", "jobid")
            name = f"model_Transformer_ddp_{job_id}_epoch_{config.total_samples}.pth"
            os.makedirs("saved_models", exist_ok=True)
            torch.save(model.state_dict(), os.path.join("saved_models", name))
            print(f"[Rank {ddp_rank}] Model saved: {name}")

        if ddp:
            dist.barrier()


# ------------------------------------------------------------------
# Evaluation / utility helpers 
# ------------------------------------------------------------------
def calculate_accuracy(preds, targets):
    exact_matches, correct_tokens, total_tokens = 0, 0, 0
    for pred, target in zip(preds, targets):
        clean_pred = [
            t
            for t in pred
            if t
            not in {
                config.start_token,
                config.end_token,
                config.pad_token,
                config.cls_token,
            }
        ]
        clean_target = [
            t
            for t in target
            if t
            not in {
                config.start_token,
                config.end_token,
                config.pad_token,
                config.cls_token,
            }
        ]
        clean_pred = clean_pred[:len(clean_target)]
        exact_matches += int(clean_pred == clean_target)

        min_len = min(len(clean_pred), len(clean_target))
        matches = sum(p == t for p, t in zip(clean_pred[:min_len], clean_target[:min_len]))
        correct_tokens += matches
        total_tokens += len(clean_target)

        if clean_pred != clean_target:
            print("not equal")
            print("clean_pred", clean_pred)
            print("clean_targ", clean_target)

    return {
        "exact": exact_matches / len(preds),
        "token": correct_tokens / total_tokens if total_tokens else 0,
    }


def test(model, dataset, return_samples=False, stdout=False):
    model.eval()
    all_preds, all_targets, sample_examples = [], [], []

    with torch.no_grad():
        for i in range(0, len(dataset), config.batch_size):
            print(f"[TEST] ===== Batch start idx={i} "
                  f"({i//config.batch_size+1}/{(len(dataset)-1)//config.batch_size+1}) =====")

            batch = dataset[i: i + config.batch_size]
            if not batch:
                print("[TEST] Empty batch – skipping")
                continue

            orig_exprs, inputs, targets = [], [], []
            for expr in batch:
                
                    orig_exprs.append(expr)
                    flat = flatten_expr(expr)
                    node_ids = [e["node_id"] for e in flat]
                    tgt_seq = [config.start_token] + node_ids + [config.end_token]
                    inputs.append(node_ids)
                    targets.append(tgt_seq)


            max_src = max(map(len, inputs))
            max_tgt = max(map(len, targets))
            pad_src, pad_tgt = [], []
            for s, t in zip(inputs, targets):
                pad_src.append(s + [config.pad_token] * (max_src - len(s)))
                pad_tgt.append(t + [config.pad_token] * (max_tgt - len(t)))

            src_nodes = torch.tensor(pad_src, dtype=torch.long, device=device)
            try:
                memory = model.encoder(src_nodes)
            except Exception as e:
                print(f"[TEST][ENCODER] Crash while encoding batch idx={i}: {e}")
                raise
            print(f"[TEST][ENCODER] Encoded batch idx={i} – memory.shape={memory.shape}")

            batch_preds = []
            for j in range(len(memory)):
                print(f"[TEST][DECODE] ► sample {j}/{len(memory)-1} in batch idx={i}")
                partial = torch.tensor([[config.start_token]],
                                       dtype=torch.long, device=device)
                tokens = []
                for step in range(config.max_gen_length):
                    next_logits = model.decoder.decode_step(memory[j].unsqueeze(0),
                                                            partial)
                    nxt = next_logits.argmax(dim=-1).item()
                    if step % 50 == 0:
                        print(f"    step={step:03d}  nxt={nxt}")
                    tokens.append(nxt)
                    if nxt == config.end_token or nxt == config.pad_token:
                        print(f"    [TEST][DECODE] sample {j} finished in {step+1} steps")
                        break
                    partial = torch.cat([partial,
                                         torch.tensor([[nxt]], device=device)], dim=1)
                else:
                    print(f"    [TEST][DECODE] ⚠️  sample {j} hit max_gen_length ({config.max_gen_length})")
                batch_preds.append(tokens)

            all_preds.extend(batch_preds)
            all_targets.extend(pad_tgt)

            if stdout:
                for o, t, p in zip(orig_exprs, pad_tgt, batch_preds):
                    print("Expression:", o)
                    print("Target    :", t[1:])
                    print("Predicted :", p)
                    print("")

            if return_samples and len(sample_examples) < 5:
                for idx in range(min(2, len(batch_preds))):
                    sample_examples.append(
                        {"target": pad_tgt[idx], "prediction": batch_preds[idx]}
                    )
        
        sample_text = ""
        for ex in sample_examples:
            sample_text += f"Target: {ex['target']}\nPred  : {ex['prediction']}\n\n"

    return calculate_accuracy(all_preds, all_targets), sample_text


def get_expression_cls_embedding(expr, model):
    flat = flatten_expr(expr)
    node_ids = [e["node_id"] for e in flat]
    if len(node_ids) + 1 > config.max_seq_length:
        return None

    src_tensor = torch.tensor([node_ids], dtype=torch.long, device=device)
    memory = model.encoder(src_tensor)
    return model.get_cls_summary(memory)


# ------------------------------------------------------------------
# Main 
# ------------------------------------------------------------------
def main():
    if sys.argv[1].lower() == "train":
        # -----------------------------------------------------------
        # PART:  each rank loads **only its shard** of the big file
        # -----------------------------------------------------------
        data_path = "/scratch/ad7786/chehab-vectorization-rl/pretraining/dataset_balanced_ROT_32_15_5000000.txt"
        # The dataset is generated using the veclang_random_gen.py
        print(f"[Rank {ddp_rank}] Loading dataset shard…")
        debug_print_memory("before dataset load")  

        local_exprs = []  # PART: only expressions for this rank
        with open(data_path, "r") as f:
            for idx, line in enumerate(f):
                if idx % ddp_world_size == ddp_rank:  
                    local_exprs.append(line.strip())

        # optional shuffle (local only)
        random.seed(52 + ddp_rank)  # PART: different seed per rank
        random.shuffle(local_exprs)

        print(f"[Rank {ddp_rank}] Shard size = {len(local_exprs)}")
        debug_print_memory("after dataset load")  

        train_dataset = [parse_sexpr(e) for e in local_exprs]
        if master_process:
            total = len(local_exprs) * ddp_world_size
            print(f"Global dataset (approx) = {total}")

        model = TRAE().to(device)
        debug_print_memory("model to(device)")  

        if ddp:
            model = AEDDP(model, device_ids=[deviceids[ddp_rank]])
            debug_print_memory("after DDP wrap")  

        train(model, train_dataset)

        if ddp:
            dist.destroy_process_group()

    elif sys.argv[1].lower() == "test":
        model = TRAE()
        state_dict = torch.load(sys.argv[2], map_location=device)
        new_sd = {k[len("module.") :] if k.startswith("module.") else k: v for k, v in state_dict.items()}
        model.load_state_dict(new_sd)
        model.to(device)

        test_expressions = []
        with open("chehab_bench.txt", "r") as f:
            for line in f:
                test_expressions.append(line.strip())

        test_dataset = [parse_sexpr(e) for e in test_expressions if len(tokenize(e)) <= 20000  ][:-1]

            

        print("Starting evaluation…")
        acc, txt = test(model, test_dataset, stdout=True)
        print("\nFinal Test Results")
        print(f"Exact-match accuracy : {acc['exact']*100:.2f}%")
        print(f"Token-level accuracy : {acc['token']*100:.2f}%")
        print(txt)


def parameter_counts(model):
    total_params = sum(p.numel() for p in model.parameters())
    trainable_params = sum(p.numel() for p in model.parameters() if p.requires_grad)
    non_trainable_params = total_params - trainable_params
    return trainable_params, non_trainable_params


def demo():
    model = TRAE()
    model.eval()
    state_dict = torch.load("./fhe_rl/trained_models/model_Transformer_ddp_10399047_epoch_5000000.pth", map_location=device)
    new_sd = {k[len("module.") :] if k.startswith("module.") else k: v for k, v in state_dict.items()}
    model.load_state_dict(new_sd)
    model.to(device)

    
    trainable, frozen = parameter_counts(model)
    print(f"[Rank {ddp_rank}] Trainable params     : {trainable:,}")
    print(f"[Rank {ddp_rank}] Non-trainable params : {frozen:,}")
    
    expr_str1 = "(VecAdd (Vec v2_0 v2_1 v2_2 0 0 0 0 0 0) (VecAdd (Vec 3 3 3 0 0 0 0 0 0) (VecMul (Vec v1_0 v1_1 v1_2 0 0 0 0 0 0) (Vec 5 5 5 0 0 0 0 0 0))))"
    

    expr_str2 = "(VecAdd (Vec a b c 0 0 0 0 0 0) (VecAdd (Vec 3 3 3 0 0 0 0 0 0) (VecMul (Vec v1_0 v1_1 v1_2 0 0 0 0 0 0) (Vec 5 5 5 0 0 0 0 0 0))))"
    
    expr1 = parse_sexpr(expr_str1)
    expr2 = parse_sexpr(expr_str2)
    cls_vector1 = get_expression_cls_embedding(expr1, model)
    cls_vector2 = get_expression_cls_embedding(expr2, model)


    
    # Check exact equality
    are_equal = torch.all(cls_vector1 == cls_vector2).item()
    print(f"Embeddings are exactly equal: {are_equal}")
    
    # Check similarity
    cosine_similarity = torch.nn.functional.cosine_similarity(cls_vector1, cls_vector2, dim=1).item()
    l2_distance = torch.norm(cls_vector1 - cls_vector2).item()
    
    # print(f"Cosine similarity: {cosine_similarity:.6f}")
    # print(f"L2 distance: {l2_distance:.6f}")
    

if __name__ == "__main__":
    demo()
