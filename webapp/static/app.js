const benchmarkSelect = document.getElementById("benchmark");
const slotCountInput = document.getElementById("slot_count");
const windowInput = document.getElementById("window");
const cseInput = document.getElementById("cse");
const cfInput = document.getElementById("const_folding");
const quantifierInput = document.getElementById("call_quantifier");
const vectorizeInput = document.getElementById("vectorize_code");
const runBtn = document.getElementById("run-btn");
const statusBadge = document.getElementById("status");
const metricsEl = document.getElementById("metrics");
const initialExprEl = document.getElementById("initial-expr");
const optimizedExprEl = document.getElementById("optimized-expr");
const vectorSizesEl = document.getElementById("vector-sizes");
const logsEl = document.getElementById("logs");
const form = document.getElementById("run-form");
const polyFields = document.getElementById("poly-fields");
const polyDepth = document.getElementById("poly_depth");
const polyIteration = document.getElementById("poly_iteration");
const polyRegime = document.getElementById("poly_regime");

const status = (state, text) => {
  statusBadge.className = `status ${state}`;
  statusBadge.textContent = text;
};

const metric = (label, value) => {
  return `<div class="metric"><div class="label">${label}</div><div class="value">${value ?? "n/a"}</div></div>`;
};

const renderMetrics = (data) => {
  metricsEl.innerHTML = [
    metric("Compile time (ms)", data.compile_ms),
    metric("Execution (ms)", data.execution_ms),
    metric("Depth", data.depth),
    metric("Multiplicative depth", data.multiplicative_depth),
    metric("Noise budget", data.remaining_noise_budget),
  ].join("");
};

const renderIR = (data) => {
  initialExprEl.textContent = data.initial_expr || "No expression captured.";
  optimizedExprEl.textContent = data.optimized_expr || "No optimized expression captured.";
  if (data.vector_sizes && data.vector_sizes.length) {
    vectorSizesEl.textContent = `Vector widths: ${data.vector_sizes.join(", ")}`;
  } else {
    vectorSizesEl.textContent = "";
  }
};

const renderLogs = (data) => {
  logsEl.innerHTML = "";
  if (!data.logs || !data.logs.length) {
    logsEl.textContent = "No logs yet.";
    return;
  }
  data.logs.forEach((entry) => {
    const block = document.createElement("div");
    block.className = "log-entry";
    const title = document.createElement("h4");
    title.textContent = entry.step;
    const out = document.createElement("pre");
    out.className = "log-text stdout";
    out.textContent = (entry.stdout || "").trim();
    const err = document.createElement("pre");
    err.className = "log-text stderr";
    err.textContent = (entry.stderr || "").trim();
    block.appendChild(title);
    if (out.textContent) block.appendChild(out);
    if (err.textContent) block.appendChild(err);
    logsEl.appendChild(block);
  });
};

const parseErrorResponse = async (res) => {
  const text = await res.text();
  try {
    const parsed = JSON.parse(text);
    // FastAPI wraps the payload under "detail"
    if (parsed.detail) return parsed.detail;
    return parsed;
  } catch {
    return text || "Run failed";
  }
};

const togglePolyFields = (benchmark) => {
  const isPoly = benchmark === "polynomials_coyote";
  polyFields.classList.toggle("hidden", !isPoly);
};

const fillSelect = (el, values, stringify = true) => {
  if (!el) return;
  el.innerHTML = (values || [])
    .map((v) => {
      let label = v;
      if (stringify && (v === 0 || v === 1)) {
        label = v === 1 ? "enabled" : "disabled";
      }
      return `<option value="${v}">${label}</option>`;
    })
    .join("");
};

const loadBenchmarks = async () => {
  try {
    const res = await fetch("/api/benchmarks");
    const payload = await res.json();
    benchmarkSelect.innerHTML = payload.benchmarks
      .map((b) => `<option value="${b}">${b}</option>`)
      .join("");
    if (payload.defaults && payload.defaults.slot_counts?.length) {
      fillSelect(slotCountInput, payload.defaults.slot_counts);
      slotCountInput.value = payload.defaults.slot_counts[1] || payload.defaults.slot_counts[0];
    }
    if (payload.defaults?.window_options?.length) {
      // window should display raw numbers, not enabled/disabled
      fillSelect(windowInput, payload.defaults.window_options, false);
      windowInput.value = payload.defaults.window_options[0];
    }
    if (payload.defaults?.flag_options?.length) {
      const flags = payload.defaults.flag_options;
      [vectorizeInput, quantifierInput, cseInput, cfInput].forEach((el) => fillSelect(el, flags));
      vectorizeInput.value = payload.defaults.vectorize_code;
      quantifierInput.value = payload.defaults.call_quantifier;
      cseInput.value = payload.defaults.cse;
      cfInput.value = payload.defaults.const_folding;
    }
    if (payload.defaults?.polynomials?.regimes?.length) {
      polyRegime.innerHTML = payload.defaults.polynomials.regimes
        .map((r) => `<option value="${r}">${r}</option>`)
        .join("");
      if (payload.defaults.polynomials.depth_options) {
        fillSelect(polyDepth, payload.defaults.polynomials.depth_options);
      }
      if (payload.defaults.polynomials.iteration_options) {
        fillSelect(polyIteration, payload.defaults.polynomials.iteration_options);
      }
      polyDepth.value = payload.defaults.polynomials.depth ?? polyDepth.value;
      polyIteration.value = payload.defaults.polynomials.iteration ?? polyIteration.value;
      if (payload.defaults.polynomials.regime) {
        polyRegime.value = payload.defaults.polynomials.regime;
      }
    }
    togglePolyFields(benchmarkSelect.value);
  } catch (err) {
    console.error(err);
  }
};

benchmarkSelect.addEventListener("change", (ev) => {
  togglePolyFields(ev.target.value);
});

form.addEventListener("submit", async (ev) => {
  ev.preventDefault();
  status("running", "Running");
  runBtn.disabled = true;
  runBtn.textContent = "Running...";
  metricsEl.innerHTML = "";
  initialExprEl.textContent = "";
  optimizedExprEl.textContent = "";
  logsEl.textContent = "Executing commands...";

  const payload = {
    benchmark: benchmarkSelect.value,
    slot_count: Number(slotCountInput.value),
    optimization_method: 1,
    vectorize_code: Number(vectorizeInput.value),
    window: Number(windowInput.value),
    call_quantifier: Number(quantifierInput.value),
    cse: Number(cseInput.value),
    const_folding: Number(cfInput.value),
  };
  if (benchmarkSelect.value === "polynomials_coyote") {
    payload.depth = Number(polyDepth.value);
    payload.iteration = Number(polyIteration.value);
    payload.regime = polyRegime.value;
  }

  try {
    const res = await fetch("/api/run", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payload),
    });
    if (!res.ok) {
      const errPayload = await parseErrorResponse(res);
      throw errPayload;
    }
    const data = await res.json();
    renderMetrics(data);
    renderIR(data);
    renderLogs(data);
    status("done", "Done");
  } catch (err) {
    status("error", "Error");
    if (err && err.logs && err.logs.length) {
      renderLogs({ logs: err.logs });
    } else {
      const msg = err?.error || err?.message || String(err || "Run failed");
      logsEl.textContent = msg;
    }
  } finally {
    runBtn.disabled = false;
    runBtn.textContent = "Run benchmark";
  }
});

loadBenchmarks();
