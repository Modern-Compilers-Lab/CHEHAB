from stable_baselines3 import PPO
import torch.nn as nn


class HierarchicalPPO(PPO):
    def __init__(self, *args, rule_dim=5, max_positions=32, **kwargs):
        
        if "policy_kwargs" not in kwargs:
            kwargs["policy_kwargs"] = {}
        kwargs["policy_kwargs"]["rule_dim"] = rule_dim
        kwargs["policy_kwargs"]["max_positions"] = max_positions
        super().__init__(*args, **kwargs)
        self.ep_info_buffer = []

    


    def _update_info_buffer(self, infos, dones=None):
        return super()._update_info_buffer(infos, dones)

    def obs_to_tensor(self,obs, device):
        """
        Convert observation to PyTorch tensor.
        """
        if isinstance(obs, dict):
            return {
                key: torch.as_tensor(value).to(device)
                for key, value in obs.items()
            }
        return torch.as_tensor(obs).to(device)
    def collect_rollouts(
        self,
        env,
        callback,
        rollout_buffer,
        n_rollout_steps: int,
    ) -> bool:
        print("called")
        assert self._last_obs is not None, "No previous observation was provided"
        n_steps = 0
        rollout_buffer.reset()

        self.ep_info_buffer = []
        print("Cleared ep_info_buffer")

        callback.on_rollout_start()
        while n_steps < n_rollout_steps:
            if self.use_sde and self.sde_sample_freq > 0 and n_steps % self.sde_sample_freq == 0:
                self.policy.reset_noise(env.num_envs)

            with torch.no_grad():
                obs_tensor = self.obs_to_tensor(self._last_obs, self.device)
                actions, values, log_probs = self.policy.forward(obs_tensor)
            actions = actions.cpu().numpy()

            clipped_actions = actions
            if isinstance(self.action_space, gym.spaces.Box):
                clipped_actions = np.clip(actions, self.action_space.low, self.action_space.high)

            step_result = env.step(clipped_actions)
            if len(step_result) == 4:
                new_obs, rewards, dones, infos = step_result
                truncated = dones
            else:
                new_obs, rewards, dones, truncated, infos = step_result

            self.num_timesteps += env.num_envs

            for info in infos:
                if "episode" in info:
                    self.ep_info_buffer.append(info["episode"])


            callback.update_locals(locals())
            if callback.on_step() is False:
                return False

            n_steps += 1

            if isinstance(self.action_space, gym.spaces.Discrete):
                actions = actions.reshape(-1, 1)

            for idx, done in enumerate(dones):
                if (
                    done
                    and infos[idx].get("terminal_observation") is not None
                    and infos[idx].get("TimeLimit.truncated", False)
                ):
                    terminal_obs = self.obs_to_tensor(infos[idx]["terminal_observation"], self.device)
                    with torch.no_grad():
                        terminal_value = self.policy.predict_values(terminal_obs)[0]
                    rewards[idx] += self.gamma * terminal_value

            rollout_buffer.add(
                self._last_obs,
                actions,
                rewards,
                self._last_episode_starts,
                values,
                log_probs,
            )
            self._last_obs = new_obs
            self._last_episode_starts = dones

        with torch.no_grad():
            obs_tensor = self.obs_to_tensor(new_obs, self.device)
            values = self.policy.predict_values(obs_tensor)

        rollout_buffer.compute_returns_and_advantage(last_values=values, dones=dones)
        print(f"End of collection. Buffer size: {len(self.ep_info_buffer)}")
        callback.on_rollout_end()

        return True

    def train(self) -> None:
        """
        One complete PPO update.  We iterate over *all* epochs and
        *all* mini‑batches, computing a **single scalar loss**
        that combines policy/value/entropy terms.  We then call
        `.zero_grad()`, `loss.backward()`, `clip_grad_norm_()`,
        and finally `.step()` on the three optimizers.
        """

        self.rollout_buffer.advantages = (self.rollout_buffer.advantages -
                                          self.rollout_buffer.advantages.mean()) / (
                                              self.rollout_buffer.advantages.std() + 1e-8
                                          )

        logger = self.logger
        policy = self.policy
        rb = self.rollout_buffer

        last_policy_loss = last_value_loss = last_entropy_loss = 0.0
        old_values_for_ev, returns_for_ev = None, None

        for _ in range(self.n_epochs):
            for rollout_data in rb.get(batch_size=self.batch_size):

                
                if isinstance(rollout_data.observations, dict):
                    obs_dict = {
                        "observation": rollout_data.observations["observation"],
                        "action_mask": rollout_data.observations["action_mask"],
                    }
                    action_masks = rollout_data.observations["action_mask"]
                else:
                    obs_dict = rollout_data.observations
                    action_masks = None

                actions     = rollout_data.actions
                old_logprob = rollout_data.old_log_prob
                old_values  = rollout_data.old_values
                returns     = rollout_data.returns
                adv         = rollout_data.advantages

                old_values_for_ev, returns_for_ev = old_values, returns

               
                values, log_prob, entropy = policy.evaluate_actions(
                    obs_dict, actions, action_masks=action_masks
                )

                
                ratio = torch.exp(log_prob - old_logprob)
                clip_range = (
                self.clip_range(self._current_progress_remaining)
                if callable(self.clip_range)
                else self.clip_range
                )
                pg_loss = -torch.min(
                    ratio * adv,
                    torch.clamp(ratio, 1.0 - clip_range, 1.0 + clip_range) * adv,
                ).mean()

                entropy_loss = -entropy.mean()
                clip_range_vf = (
                self.clip_range_vf(self._current_progress_remaining)
                if callable(self.clip_range_vf)
                else self.clip_range_vf
                )

                vf_unclipped = (values - returns).pow(2)

                values_clipped = old_values + torch.clamp(
                values - old_values,
                -clip_range_vf,
                clip_range_vf
                )
                vf_clipped = (values_clipped - returns).pow(2)

                value_loss = 0.5 * torch.max(vf_unclipped, vf_clipped).mean()

                loss = pg_loss + self.vf_coef * value_loss + policy.ent_coef * entropy_loss

                
                policy.rule_optimizer.zero_grad()
                policy.position_optimizer.zero_grad()
                policy.value_optimizer.zero_grad()

                loss.backward()
                nn.utils.clip_grad_norm_(policy.parameters(), self.max_grad_norm)

                policy.rule_optimizer.step()
                policy.position_optimizer.step()
                policy.value_optimizer.step()

                last_policy_loss  = pg_loss.detach()
                last_value_loss   = value_loss.detach()
                last_entropy_loss = entropy_loss.detach()

       
        if old_values_for_ev is not None:
            ev = explained_variance(
                old_values_for_ev.cpu().numpy().flatten(),
                returns_for_ev.cpu().numpy().flatten(),
            )
            logger.record("train/explained_variance", ev)

        logger.record("train/policy_loss",  last_policy_loss.item())
        logger.record("train/value_loss",   last_value_loss.item())
        logger.record("train/entropy_loss", last_entropy_loss.item())
        logger.record("train/entropy_coef", policy.ent_coef)

