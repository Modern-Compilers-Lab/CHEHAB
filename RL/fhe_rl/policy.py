import torch, torch.nn as nn
from torch.distributions import Categorical
from .utils import mlp
from typing import Dict, Tuple
class CustomFeaturesExtractor(nn.Module):
    def __init__(self, observation_space, features_dim: int = 256):
        super().__init__()
        self._embed_dim = observation_space["observation"].shape[0]
        self._features_dim = features_dim
    def forward(self, obs_dict):
        return obs_dict["observation"]
    @property
    def features_dim(self):
        return self._features_dim

class HierarchicalMaskablePolicy(nn.Module):
    def __init__(self, observation_space, action_space, lr_schedule, **policy_kwargs):
        """
        Custom hierarchical policy that selects a high-level rule and then a sub-action (position).
        Expected extra parameters (via policy_kwargs):
          - rule_dim: Total number of high-level rules.
          - max_positions: Number of positions (sub-actions) available per rule.
          - features_dim: (Optional) Dimension of feature extractor output (default: 256).
          - lr: Learning rate for the separate optimizers (default: 3e-4).
          - ent_coef: Entropy coefficient (default: 0.01).
        """
        super().__init__()
        self.rule_dim = policy_kwargs.pop("rule_dim", 5)
        self.max_positions = policy_kwargs.pop("max_positions", 2)
        features_dim = policy_kwargs.pop("features_dim", 256)
        lr = policy_kwargs.pop("lr", 3e-4)
        rule_hidden_dims  = policy_kwargs.pop("rule_hidden_dims",  [128, 128])
        pos_hidden_dims   = policy_kwargs.pop("pos_hidden_dims",   [128, 128])
        value_hidden_dims = policy_kwargs.pop("value_hidden_dims", [256, 128, 64])
        
        self.encoder = CustomFeaturesExtractor(observation_space, features_dim)

        self.rule_head = mlp(
            in_dim      = features_dim,
            hidden_dims = rule_hidden_dims,
            out_dim     = self.rule_dim,
            act         = nn.ReLU,
            layernorm   = True,
        )

        self.pos_head = mlp(
            in_dim      = features_dim + self.rule_dim,
            hidden_dims = pos_hidden_dims,
            out_dim     = self.max_positions,
            act         = nn.ReLU,
            layernorm   = True,
        )

        self.value_net = mlp(
            in_dim      = features_dim,
            hidden_dims = value_hidden_dims,
            out_dim     = 1,
            act         = nn.ReLU,
            layernorm   = True,
        )
        
        actor_params  = (
            list(self.encoder.parameters())
            + list(self.rule_head.parameters())
            + list(self.pos_head.parameters())
        )
        critic_params = self.value_net.parameters()
        self.optimizer = torch.optim.Adam(
            [
                {"params": actor_params,  "lr": lr},
                {"params": critic_params, "lr": lr * 0.1},
            ]
        )
    def _rule_dist(self, enc: torch.Tensor, rule_mask: torch.Tensor) -> Categorical:
        logits = self.rule_head(enc)
        logits = torch.where(rule_mask, logits, torch.tensor(-torch.inf, device=enc.device))
        return Categorical(logits=logits)

    def _pos_dist(
        self, enc: torch.Tensor, one_hot_rule: torch.Tensor, pos_mask: torch.Tensor
    ) -> Categorical:
        logits = self.pos_head(torch.cat([enc, one_hot_rule], dim=1))
        logits = torch.where(pos_mask, logits, torch.tensor(-torch.inf, device=enc.device))
        return Categorical(logits=logits)
    def predict(
        self,
        observation,
        state=None,
        episode_start=None,
        deterministic: bool = False,
    ):
        """Minimal imitation of ActorCriticPolicy.predict()."""
        device = next(self.parameters()).device
        if isinstance(observation, dict):
            obs = {k: torch.as_tensor(v, device=device) for k, v in observation.items()}
        else:
            obs = torch.as_tensor(observation, device=device)
        actions, _, _ = self.forward(obs, deterministic=deterministic)
        return actions.cpu().numpy(), state
    
    def forward(
        self,
        obs: Dict[str, torch.Tensor],
        deterministic: bool = False,
    ) -> Tuple[torch.Tensor, torch.Tensor, torch.Tensor]:
        """Return (actions, values, log_prob)."""
        enc = self.encoder(obs)

        mask = obs["action_mask"].bool()
        B = mask.size(0)
        mask = mask.view(B, self.rule_dim, self.max_positions)

        rule_mask = mask.any(dim=2)
        rule_dist = self._rule_dist(enc, rule_mask)
        rule_action = rule_dist.mode if deterministic else rule_dist.sample()

        one_hot_rule = torch.nn.functional.one_hot(rule_action, self.rule_dim).float()
        pos_mask = mask[torch.arange(B, device=enc.device), rule_action]
        pos_dist = self._pos_dist(enc, one_hot_rule, pos_mask)
        pos_action = pos_dist.mode if deterministic else pos_dist.sample()

        flat_action = rule_action * self.max_positions + pos_action
        log_prob = rule_dist.log_prob(rule_action) + pos_dist.log_prob(pos_action)

        value = self.value_net(enc).squeeze(-1)
        return flat_action, value, log_prob

    def evaluate_actions(
        self,
        obs: Dict[str, torch.Tensor],
        actions: torch.Tensor,
    ) -> Tuple[torch.Tensor, torch.Tensor, torch.Tensor]:
        """Used inside PPO.update; returns (values, log_prob, entropy)."""
        enc = self.encoder(obs)
        B = actions.size(0)
        mask = obs["action_mask"].bool().view(B, self.rule_dim, self.max_positions)

        rule_actions = (actions // self.max_positions).long()
        pos_actions = (actions % self.max_positions).long()

        rule_mask = mask.any(dim=2)
        rule_dist = self._rule_dist(enc, rule_mask)
        one_hot_rule = torch.nn.functional.one_hot(rule_actions, self.rule_dim).float()

        pos_mask = mask[torch.arange(B, device=enc.device), rule_actions]
        pos_dist = self._pos_dist(enc, one_hot_rule, pos_mask)

        log_prob = rule_dist.log_prob(rule_actions) + pos_dist.log_prob(pos_actions)
        entropy = rule_dist.entropy() + pos_dist.entropy()
        value = self.value_net(enc).squeeze(-1)
        return value, log_prob, entropy
    
    def predict_values(self, obs: Dict[str, torch.Tensor]) -> torch.Tensor:
        enc = self.encoder(obs)
        return self.value_net(enc).squeeze(-1)

    def set_training_mode(self, mode: bool):
        self.train(mode)
