# Builder Dockerfile
# Multi-stage build for creating wheels

FROM rust:latest as builder

# Install build dependencies including Python and pip
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    python3 \
    python3-dev \
    python3-pip \
    libicu-dev \
    && rm -rf /var/lib/apt/lists/*

# Install maturin
RUN pip3 install --break-system-packages maturin

# Create app directory
WORKDIR /build

# Copy source code
COPY . .

# Build Rust crate
RUN cd rust && cargo build --release

# Build Python wheels for multiple Python versions
RUN cd python && \
    maturin build --release --interpreter python3.8 python3.9 python3.10 python3.11

# Export wheels
FROM scratch as export
COPY --from=builder /build/python/target/wheels/*.whl /
