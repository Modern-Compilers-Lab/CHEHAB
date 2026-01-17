FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Build/toolchain dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential cmake ninja-build git curl pkg-config \
    libgmp-dev libssl-dev libtinfo-dev \
    clang lld \
    rustc cargo \
    wget ca-certificates \
    bzip2 && \
    rm -rf /var/lib/apt/lists/*

# Microsoft SEAL
RUN git clone https://github.com/microsoft/SEAL.git /tmp/SEAL && \
    cmake -S /tmp/SEAL -B /tmp/SEAL/build -DCMAKE_INSTALL_PREFIX=/opt/seal -DCMAKE_BUILD_TYPE=Release && \
    cmake --build /tmp/SEAL/build --config Release && \
    cmake --install /tmp/SEAL/build && \
    rm -rf /tmp/SEAL


ENV CMAKE_PREFIX_PATH=/opt/seal
ENV LD_LIBRARY_PATH=/opt/seal/lib:${LD_LIBRARY_PATH}
ENV PATH="/root/.cargo/bin:${PATH}"
ENV CC=/usr/bin/gcc
ENV CXX=/usr/bin/g++

# Miniconda
ENV CONDA_DIR=/opt/conda
RUN wget --quiet https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O /tmp/miniconda.sh && \
    /bin/bash /tmp/miniconda.sh -b -p $CONDA_DIR && \
    rm /tmp/miniconda.sh
ENV PATH=$CONDA_DIR/bin:$PATH

SHELL ["/bin/bash", "-c"]

WORKDIR /app

# Copy only dependency manifests first to maximize Docker layer caching.
COPY environment.yml /app/environment.yml
COPY RL/pytrs /app/RL/pytrs

# Accept Conda Terms of Service
RUN conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/main \
    && conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/r

# Create Conda environment
RUN conda env create -f environment.yml && conda clean -afy

RUN conda run -n chehabEnv pip install --no-cache-dir -e RL/pytrs

# Now copy the rest of the repository (changes here won't invalidate the conda env layer).
COPY . /app

# Optional in-image compiler build (set SKIP_BUILD=1 to skip)
ARG SKIP_BUILD=0
RUN if [ "$SKIP_BUILD" -eq "0" ]; then \
    conda run -n chehabEnv /bin/bash -c "cmake -S . -B build && cmake --build build"; \
    fi

# Drop users into a shell with the environment activated
RUN echo "source $CONDA_DIR/etc/profile.d/conda.sh && conda activate chehabEnv" >> /root/.bashrc

CMD ["/bin/bash"]
