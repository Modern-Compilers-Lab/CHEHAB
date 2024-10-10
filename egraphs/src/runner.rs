use std::fmt::{self, Debug, Formatter};

use log::*;

use egg::*;
use indexmap::IndexMap;
use std::time::Duration;
use std::time::Instant;
use std::collections::hash_map::RandomState;

pub struct Runner<L: Language, N: Analysis<L>, IterData = ()> {
    /// The [`EGraph`] used.
    pub egraph: EGraph<L, N>,
    /// Data accumulated over each [`Iteration`].
    pub iterations: Vec<Iteration<IterData>>,
    /// The roots of expressions added by the
    /// [`with_expr`](Runner::with_expr()) method, in insertion order.
    pub roots: Vec<Id>,
    /// Why the `Runner` stopped. This will be `None` if it hasn't
    /// stopped yet.
    pub stop_reason: Option<StopReason>,

    /// The hooks added by the
    /// [`with_hook`](Runner::with_hook()) method, in insertion order.
    #[allow(clippy::type_complexity)]
    pub hooks: Vec<Box<dyn FnMut(&mut Self) -> Result<(), String>>>,

    // limits
    iter_limit: usize,
    node_limit: usize,
    time_limit: Duration,

    start_time: Option<Instant>,
    scheduler: Box<dyn RewriteScheduler<L, N>>,
}

impl<L, N> Default for Runner<L, N, ()>
where
    L: Language,
    N: Analysis<L> + Default,
{
    fn default() -> Self {
        eprintln!("Hello runner");
        Runner::new(N::default())
    }
}

impl<L, N, IterData> Debug for Runner<L, N, IterData>
where
    L: Language,
    N: Analysis<L>,
    IterData: Debug,
{
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        // Use an exhaustive pattern match to ensure the Debug implementation and the struct stay in sync.
        let Runner {
            egraph,
            iterations,
            roots,
            stop_reason,
            hooks,
            iter_limit,
            node_limit,
            time_limit,
            start_time,
            scheduler: _,
        } = self;

        f.debug_struct("Runner")
            .field("egraph", egraph)
            .field("iterations", iterations)
            .field("roots", roots)
            .field("stop_reason", stop_reason)
            .field("hooks", &vec![format_args!("<dyn FnMut ..>"); hooks.len()])
            .field("iter_limit", iter_limit)
            .field("node_limit", node_limit)
            .field("time_limit", time_limit)
            .field("start_time", start_time)
            .field("scheduler", &format_args!("<dyn RewriteScheduler ..>"))
            .finish()
    }
}

/// Error returned by [`Runner`] when it stops.
///
#[derive(Debug, Clone)]
pub enum StopReason {
    /// The egraph saturated, i.e., there was an iteration where we
    /// didn't learn anything new from applying the rules.
    Saturated,
    /// The iteration limit was hit. The data is the iteration limit.
    IterationLimit(usize),
    /// The enode limit was hit. The data is the enode limit.
    NodeLimit(usize),
    /// The time limit was hit. The data is the time limit in seconds.
    TimeLimit(f64),
    /// Some other reason to stop.
    Other(String),
}

/// A report containing data about an entire [`Runner`] run.
///
/// This is basically a summary of the [`Iteration`] data,
/// but summed across iterations.
/// See [`Iteration`] docs for details about fields.
#[derive(Debug, Clone)]
#[non_exhaustive]
#[allow(missing_docs)]
pub struct Report {
    /// The number of iterations this runner performed.
    pub iterations: usize,
    pub stop_reason: StopReason,
    pub egraph_nodes: usize,
    pub egraph_classes: usize,
    pub memo_size: usize,
    pub rebuilds: usize,
    pub total_time: f64,
    pub search_time: f64,
    pub apply_time: f64,
    pub rebuild_time: f64,
}

impl std::fmt::Display for Report {
    #[rustfmt::skip]
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        writeln!(f, "Runner report")?;
        writeln!(f, "=============")?;
        writeln!(f, "  Stop reason: {:?}", self.stop_reason)?;
        writeln!(f, "  Iterations: {}", self.iterations)?;
        writeln!(f, "  Egraph size: {} nodes, {} classes, {} memo", self.egraph_nodes, self.egraph_classes, self.memo_size)?;
        writeln!(f, "  Rebuilds: {}", self.rebuilds)?;
        writeln!(f, "  Total time: {}", self.total_time)?;
        writeln!(f, "    Search:  ({:.2}) {}", self.search_time / self.total_time, self.search_time)?;
        writeln!(f, "    Apply:   ({:.2}) {}", self.apply_time / self.total_time, self.apply_time)?;
        writeln!(f, "    Rebuild: ({:.2}) {}", self.rebuild_time / self.total_time, self.rebuild_time)?;
        Ok(())
    }
}

#[derive(Debug, Clone)]
#[non_exhaustive]
pub struct Iteration<IterData> {
    /// The number of enodes in the egraph at the start of this
    /// iteration.
    pub egraph_nodes: usize,
    /// The number of eclasses in the egraph at the start of this
    /// iteration.
    pub egraph_classes: usize,
    /// A map from rule name to number of times it was _newly_ applied
    /// in this iteration.
    pub applied: IndexMap<Symbol, usize>,
    /// Seconds spent running hooks.
    pub hook_time: f64,
    /// Seconds spent searching in this iteration.
    pub search_time: f64,
    /// Seconds spent applying rules in this iteration.
    pub apply_time: f64,
    /// Seconds spent [`rebuild`](EGraph::rebuild())ing
    /// the egraph in this iteration.
    pub rebuild_time: f64,
    /// Total time spent in this iteration, including data generation time.
    pub total_time: f64,
    /// The user provided annotation for this iteration
    pub data: IterData,
    /// The number of rebuild iterations done after this iteration completed.
    pub n_rebuilds: usize,
    /// If the runner stopped on this iterations, this is the reason
    pub stop_reason: Option<StopReason>,
}

type RunnerResult<T> = std::result::Result<T, StopReason>;

impl<L, N, IterData> Runner<L, N, IterData>
where
    L: Language,
    N: Analysis<L>,
    IterData: IterationData<L, N>,
{
    /// Create a new `Runner` with the given analysis and default parameters.
    pub fn new(analysis: N) -> Self {
        debug!("Hello runner");
        Self {
            iter_limit: 30,
            node_limit: 10_000,
            time_limit: Duration::from_secs(5),

            egraph: EGraph::new(analysis),
            roots: vec![],
            iterations: vec![],
            stop_reason: None,
            hooks: vec![],

            start_time: None,
            scheduler: Box::new(BackoffScheduler::new(
                /* match_limit */ 100_000,
                /* ban_length */ 2,
            )),
        }
    }

    /// Sets the iteration limit. Default: 30
    pub fn with_iter_limit(self, iter_limit: usize) -> Self {
        Self { iter_limit, ..self }
    }

    /// Sets the egraph size limit (in enodes). Default: 10,000
    pub fn with_node_limit(self, node_limit: usize) -> Self {
        Self { node_limit, ..self }
    }

    /// Sets the runner time limit. Default: 5 seconds
    pub fn with_time_limit(self, time_limit: Duration) -> Self {
        Self { time_limit, ..self }
    }

    /// Add a hook to instrument or modify the behavior of a [`Runner`].
    /// Each hook will run at the beginning of each iteration, i.e. before
    /// all the rewrites.
    ///
    pub fn with_hook<F>(mut self, hook: F) -> Self
    where
        F: FnMut(&mut Self) -> Result<(), String> + 'static,
    {
        self.hooks.push(Box::new(hook));
        self
    }

    /// Change out the [`RewriteScheduler`] used by this [`Runner`].
    /// The default one is [`BackoffScheduler`].
    ///
    pub fn with_scheduler(self, scheduler: impl RewriteScheduler<L, N> + 'static) -> Self {
        let scheduler = Box::new(scheduler);
        Self { scheduler, ..self }
    }

    /// Add an expression to the egraph to be run.
    ///
    /// The eclass id of this addition will be recorded in the
    /// [`roots`](Runner::roots) field, ordered by
    /// insertion order.
    pub fn with_expr(mut self, expr: &RecExpr<L>) -> Self {
        let id = self.egraph.add_expr(expr);
        self.roots.push(id);
        self
    }

    /// Replace the [`EGraph`] of this `Runner`.
    pub fn with_egraph(self, egraph: EGraph<L, N>) -> Self {
        Self { egraph, ..self }
    }

    /// Run this `Runner` until it stops.
    /// After this, the field
    /// [`stop_reason`](Runner::stop_reason) is guaranteed to be
    /// set.
    pub fn run<'a, R>(mut self, rules: R) -> Self
    where
        R: IntoIterator<Item = &'a Rewrite<L, N>>,
        L: 'a,
        N: 'a,
    {
        let rules: Vec<&Rewrite<L, N>> = rules.into_iter().collect();
        check_rules(&rules);
        self.egraph.rebuild();
        loop {
            let iter = self.run_one(&rules);
            self.iterations.push(iter);
            let stop_reason = self.iterations.last().unwrap().stop_reason.clone();
            // we need to check_limits after the iteration is complete to check for iter_limit
            if let Some(stop_reason) = stop_reason.or_else(|| self.check_limits().err()) {
                info!("Stopping: {:?}", stop_reason);
                self.stop_reason = Some(stop_reason);
                break;
            }
        }

        assert!(!self.iterations.is_empty());
        assert!(self.stop_reason.is_some());
        self
    }

    /// Prints some information about a runners run.
    pub fn print_report(&self) {
        println!("{}", self.report())
    }

    /// Creates a [`Report`] summarizing this `Runner`s run.
    pub fn report(&self) -> Report {
        Report {
            stop_reason: self.stop_reason.clone().unwrap(),
            iterations: self.iterations.len(),
            egraph_nodes: self.egraph.total_number_of_nodes(),
            egraph_classes: self.egraph.number_of_classes(),
            memo_size: self.egraph.total_size(),
            rebuilds: self.iterations.iter().map(|i| i.n_rebuilds).sum(),
            search_time: self.iterations.iter().map(|i| i.search_time).sum(),
            apply_time: self.iterations.iter().map(|i| i.apply_time).sum(),
            rebuild_time: self.iterations.iter().map(|i| i.rebuild_time).sum(),
            total_time: self.iterations.iter().map(|i| i.total_time).sum(),
        }
    }

    fn run_one(&mut self, rules: &[&Rewrite<L, N>]) -> Iteration<IterData> {
        assert!(self.stop_reason.is_none());

        info!("\nIteration {}", self.iterations.len());
        
        self.try_start();
        let mut result = self.check_limits();

        let egraph_nodes = self.egraph.total_size();
        let egraph_classes = self.egraph.number_of_classes();

        let hook_time = Instant::now();
        let mut hooks = std::mem::take(&mut self.hooks);
        result = result.and_then(|_| {
            hooks
                .iter_mut()
                .try_for_each(|hook| hook(self).map_err(StopReason::Other))
        });
        self.hooks = hooks;
        let hook_time = hook_time.elapsed().as_secs_f64();

        let egraph_nodes_after_hooks = self.egraph.total_size();
        let egraph_classes_after_hooks = self.egraph.number_of_classes();

        let i = self.iterations.len();
        trace!("EGraph {:?}", self.egraph.dump());

        let start_time = Instant::now();

        let mut matches = Vec::new();
        let mut applied = IndexMap::default();
        let sample_size = 5;

        result = result.and_then(|_| {
            rules.iter().try_for_each(|rw| {
                let ms = self.scheduler.search_rewrite(i, &self.egraph, rw);
                let total_matches: usize = ms.iter().map(|m| m.substs.len()).sum();

                debug!("Rewrite rule '{}' matched {} times.", rw.name, ms.len());

                if rw.name.as_str().starts_with("exp"){
                    // Expansive rule: sample matches
                    let sampled_matches: Vec<_> = ms.into_iter().take(sample_size).collect();
                    matches.push(sampled_matches);
                    debug!("Sampled {} matches for expansive rule {}", sample_size, rw.name);
                } else {
                    // Non-expansive rule: apply all matches
                    matches.push(ms);
                    debug!("Applied all {} matches for rule {}", total_matches, rw.name);
                }

                self.check_limits()
            })
        });

        let search_time = start_time.elapsed().as_secs_f64();
        info!("Search time: {}", search_time);

        let apply_time = Instant::now();

        result = result.and_then(|_| {
            rules.iter().zip(matches).try_for_each(|(rw, ms)| {
                let total_matches: usize = ms.iter().map(|m| m.substs.len()).sum();

                debug!("Applying {} {} times", rw.name, total_matches);
                if total_matches == 0 {
                    debug!("No matches for rule '{}'. Skipping application.", rw.name);
                    return Ok(());
                }

                let actually_matched = self.scheduler.apply_rewrite(i, &mut self.egraph, rw, ms);
                if actually_matched > 0 {
                    if let Some(count) = applied.get_mut(&rw.name) {
                        *count += actually_matched;
                    } else {
                        applied.insert(rw.name.to_owned(), actually_matched);
                    }
                    debug!("Applied {} {} times", rw.name, actually_matched);
                }
                self.check_limits()
            })
        });

        let apply_time = apply_time.elapsed().as_secs_f64();
        info!("Apply time: {}", apply_time);

        let rebuild_time = Instant::now();
        let n_rebuilds = self.egraph.rebuild();
        

        let rebuild_time = rebuild_time.elapsed().as_secs_f64();
        info!("Rebuild time: {}", rebuild_time);
        info!(
            "Size: n={}, e={}",
            self.egraph.total_size(),
            self.egraph.number_of_classes()
        );

        let can_be_saturated = applied.is_empty()
            && self.scheduler.can_stop(i)
            // now make sure the hooks didn't do anything
            && (egraph_nodes == egraph_nodes_after_hooks)
            && (egraph_classes == egraph_classes_after_hooks)
            // now make sure that conditional rules (which might add
            // nodes without applying) didn't do anything
            && (egraph_nodes == self.egraph.total_size())
            && (egraph_classes == self.egraph.number_of_classes());

        if can_be_saturated {
            result = result.and(Err(StopReason::Saturated))
        }

        Iteration {
            applied,
            egraph_nodes,
            egraph_classes,
            hook_time,
            search_time,
            apply_time,
            rebuild_time,
            n_rebuilds,
            data: IterData::make(self),
            total_time: start_time.elapsed().as_secs_f64(),
            stop_reason: result.err(),
        }
    }

    fn try_start(&mut self) {
        self.start_time.get_or_insert_with(Instant::now);
    }

    fn check_limits(&self) -> RunnerResult<()> {
        let elapsed = self.start_time.unwrap().elapsed();
        if elapsed > self.time_limit {
            return Err(StopReason::TimeLimit(elapsed.as_secs_f64()));
        }

        let size = self.egraph.total_size();
        if size > self.node_limit {
            return Err(StopReason::NodeLimit(size));
        }

        if self.iterations.len() >= self.iter_limit {
            return Err(StopReason::IterationLimit(self.iterations.len()));
        }

        Ok(())
    }
}

fn check_rules<L, N>(rules: &[&Rewrite<L, N>]) {
    let mut name_counts: IndexMap<egg::Symbol, usize, RandomState> = IndexMap::default();
    for rw in rules {
        *name_counts.entry(rw.name).or_default() += 1
    }

    name_counts.retain(|_, count: &mut usize| *count > 1);
    if !name_counts.is_empty() {
        eprintln!("WARNING: Duplicated rule names may affect rule reporting and scheduling.");
        log::warn!("Duplicated rule names may affect rule reporting and scheduling.");
        for (name, &count) in name_counts.iter() {
            assert!(count > 1);
            eprintln!("Rule '{}' appears {} times", name, count);
            log::warn!("Rule '{}' appears {} times", name, count);
        }
    }
}

/** A way to customize how a [`Runner`] runs [`Rewrite`]s.

This gives you a way to prevent certain [`Rewrite`]s from exploding
the [`EGraph`] and dominating how much time is spent while running the
[`Runner`].

*/
#[allow(unused_variables)]
pub trait RewriteScheduler<L, N>
where
    L: Language,
    N: Analysis<L>,
{
    /// This is only called when the runner is otherwise saturated.
    /// Default implementation just returns `true`.
    ///
    fn can_stop(&mut self, iteration: usize) -> bool {
        eprintln!("hello can stop from simple");
        true
    }

    /// A hook allowing you to customize rewrite searching behavior.
    /// Useful to implement rule management.
    ///
    fn search_rewrite<'a>(
        &mut self,
        iteration: usize,
        egraph: &EGraph<L, N>,
        rewrite: &'a Rewrite<L, N>,
    ) -> Vec<SearchMatches<'a, L>> {
        rewrite.search(egraph)
    }

    /// A hook allowing you to customize rewrite application behavior.
    /// Useful to implement rule management.
    ///
    fn apply_rewrite(
        &mut self,
        iteration: usize,
        egraph: &mut EGraph<L, N>,
        rewrite: &Rewrite<L, N>,
        matches: Vec<SearchMatches<L>>,
    ) -> usize {
        rewrite.apply(egraph, &matches).len()
    }
}   

/// A [`RewriteScheduler`] that implements exponentional rule backoff.
///
/// For each rewrite, there exists a configurable initial match limit.
/// If a rewrite search yield more than this limit, then we ban this
/// rule for number of iterations, double its limit, and double the time
/// it will be banned next time.
///
/// This seems effective at preventing explosive rules like
/// associativity from taking an unfair amount of resources.
///
/// [`BackoffScheduler`] is configurable in the builder-pattern style.
///

#[derive(Debug)]
pub struct BackoffScheduler {
    default_match_limit: usize,
    default_ban_length: usize,
    stats: IndexMap<Symbol, RuleStats>,
}


#[derive(Debug)]
struct RuleStats {
    times_applied: usize,
    banned_until: usize,
    times_banned: usize,
    match_limit: usize,
    ban_length: usize,
}

impl BackoffScheduler {

    pub fn new (match_limit: usize , ban_length: usize) -> Self {
        debug!("Creating a BackoffScheduler instance");
        Self {
            stats: Default::default(),
            default_match_limit: match_limit,
            default_ban_length: ban_length,
        }
    }

    fn rule_stats(&mut self, name: Symbol) -> &mut RuleStats {
        if self.stats.contains_key(&name) {
            &mut self.stats[&name]
        } else {
            self.stats.entry(name).or_insert(RuleStats {
                times_applied: 0,
                banned_until: 0,
                times_banned: 0,
                match_limit: self.default_match_limit,
                ban_length: self.default_ban_length,
            })
        }
    }
}

impl<L, N> RewriteScheduler<L, N> for BackoffScheduler
where
    L: Language,
    N: Analysis<L>,
{
    fn can_stop(&mut self, iteration: usize) -> bool {
        eprintln!("hello can stop from backoff");
        let n_stats = self.stats.len();

        let mut banned: Vec<_> = self
            .stats
            .iter_mut()
            .filter(|(_, s)| s.banned_until > iteration)
            .collect();

        if banned.is_empty() {
            true
        } else {
            let min_ban = banned
                .iter()
                .map(|(_, s)| s.banned_until)
                .min()
                .expect("banned cannot be empty here");

            assert!(min_ban >= iteration);
            let delta = min_ban - iteration;

            let mut unbanned = vec![];
            for (name, s) in &mut banned {
                s.banned_until -= delta;
                if s.banned_until == iteration {
                    unbanned.push(name.as_str());
                }
            }

            assert!(!unbanned.is_empty());
            debug!(
                "Banned {}/{}, fast-forwarded by {} to unban {}",
                banned.len(),
                n_stats,
                delta,
                unbanned.join(", "),
            );

            false
        }
    }
    

    fn search_rewrite<'a>(
        &mut self,
        iteration: usize,
        egraph: &EGraph<L, N>,
        rewrite: &'a Rewrite<L, N>,
    ) -> Vec<SearchMatches<'a, L>> {
        let stats = self.rule_stats(rewrite.name);
        let exp_rule = rewrite.name.as_str().starts_with("exp");

        if iteration < stats.banned_until {
            debug!(
                "Skipping {} ({}-{}), banned until {}...",
                rewrite.name, stats.times_applied, stats.times_banned, stats.banned_until,
            );
            return vec![];
        }

        let threshold = stats
            .match_limit
            .checked_shl(stats.times_banned as u32)
            .unwrap();
        let matches = rewrite.search_with_limit(egraph, threshold.saturating_add(1));
        let total_len: usize = matches.iter().map(|m| m.substs.len()).sum();
        if total_len > threshold {

            if exp_rule {
                 // For expensive rules that exceed the threshold, return the matches directly
                 debug!(
                    "Returning matches for {} without banning: {} matches exceed threshold {}",
                    rewrite.name, total_len, threshold
                );
                return matches; // Return the matches immediately

            } else {
                // Dealing non-expensive rules by the standard implementation
                let ban_length = stats.ban_length << stats.times_banned;
                stats.times_banned += 1;
                stats.banned_until = iteration + ban_length;
                debug!(
                    "Banning {} ({}-{}) for {} iters: {} < {}",
                    rewrite.name,
                    stats.times_applied,
                    stats.times_banned,
                    ban_length,
                    threshold,
                    total_len,
                );
                vec![]
            }
        } else {
            stats.times_applied += 1;
            matches
        }
    }
}


/// Custom data to inject into the [`Iteration`]s recorded by a [`Runner`]
///
/// This trait allows you to add custom data to the [`Iteration`]s
/// recorded as a [`Runner`] applies rules.
///
/// See the [`Runner`] docs for an example.
///
/// [`Runner`] is generic over the [`IterationData`] that it will be in the
/// [`Iteration`]s, but by default it uses `()`.
///
pub trait IterationData<L, N>: Sized
where
    L: Language,
    N: Analysis<L>,
{
    /// Given the current [`Runner`], make the
    /// data to be put in this [`Iteration`].
    fn make(runner: &Runner<L, N, Self>) -> Self;
}

impl<L, N> IterationData<L, N> for ()
where
    L: Language,
    N: Analysis<L>,
{
    fn make(_: &Runner<L, N, Self>) -> Self {}
}