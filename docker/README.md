# Docker Build and Usage Guide

This directory contains Docker configurations for building and running Aura Tokenizer.

## Available Dockerfiles

### 1. Dockerfile.dev
**Purpose**: Development environment with all tools installed

**Usage**:
```bash
# Build
docker build -f docker/Dockerfile.dev -t aura-tokenizer:dev .

# Run interactive shell
docker run -it -v ${PWD}:/workspace aura-tokenizer:dev

# Inside container
cd rust && cargo build
cd ../python && maturin develop
pytest tests/
```

### 2. Dockerfile.prod
**Purpose**: Minimal production image with installed package

**Usage**:
```bash
# First, build wheels using builder or dev image
docker build -f docker/Dockerfile.builder -t aura-tokenizer:builder .

# Then build production image
docker build -f docker/Dockerfile.prod -t aura-tokenizer:prod .

# Run
docker run -it aura-tokenizer:prod python3
>>> import aura_tokenizer
>>> tokenizer = aura_tokenizer.BPETokenizer()
```

### 3. Dockerfile.builder
**Purpose**: Multi-stage build for creating wheels

**Usage**:
```bash
# Build and export wheels
docker build -f docker/Dockerfile.builder --target export --output wheels .

# Wheels will be in ./wheels/ directory
ls wheels/
```

## Docker Compose

**Quick Start**:
```bash
# Development environment
docker-compose -f docker/docker-compose.yml run --rm dev

# Build wheels
docker-compose -f docker/docker-compose.yml run --rm builder

# Run tests
docker-compose -f docker/docker-compose.yml run --rm test
```

## Common Workflows

### Build Wheels for Distribution

```bash
# Using builder
docker build -f docker/Dockerfile.builder --target export --output wheels .

# Or using compose
docker-compose -f docker/docker-compose.yml run --rm builder
```

Wheels will be available in `python/target/wheels/` or `wheels/` directory.

### Development Workflow

```bash
# Start dev container
docker-compose -f docker/docker-compose.yml run --rm dev

# Inside container
cd rust
cargo build
cargo test

cd ../python
maturin develop
pytest tests/
```

### Testing

```bash
# Run all tests
docker-compose -f docker/docker-compose.yml run --rm test

# Or manually
docker run -it -v ${PWD}:/workspace aura-tokenizer:dev bash -c "
  cd /workspace/rust && cargo test &&
  cd /workspace/python && pytest tests/
"
```

## Volume Mounts

The development container mounts:
- Source code: `..:/workspace`
- Cargo cache: `cargo-cache:/usr/local/cargo/registry`
- Build cache: `target-cache:/workspace/rust/target`

This ensures fast rebuilds by caching dependencies.

## Multi-Platform Builds

To build for different architectures:

```bash
# Setup buildx
docker buildx create --use

# Build for multiple platforms
docker buildx build \
  --platform linux/amd64,linux/arm64 \
  -f docker/Dockerfile.dev \
  -t aura-tokenizer:dev \
  --push .
```

## CI/CD Integration

Use in GitHub Actions:

```yaml
- name: Build wheels in Docker
  run: |
    docker build -f docker/Dockerfile.builder --target export --output wheels .

- name: Upload wheels
  uses: actions/upload-artifact@v3
  with:
    name: wheels
    path: wheels/*.whl
```

## Troubleshooting

### Slow builds

Enable BuildKit for faster builds:
```bash
export DOCKER_BUILDKIT=1
docker build -f docker/Dockerfile.dev .
```

### Permission issues

If you encounter permission issues with mounted volumes:
```bash
# Build with your user ID
docker build --build-arg UID=$(id -u) --build-arg GID=$(id -g) -f docker/Dockerfile.dev .
```

### Cache issues

Clear Docker cache:
```bash
docker builder prune -a
docker volume prune
```

## Best Practices

1. **Use .dockerignore**: Already configured to exclude `target/` and other build artifacts
2. **Layer caching**: Dependencies are installed before copying source code
3. **Multi-stage builds**: Keep final images small
4. **Volume mounts**: Use for development to avoid rebuilding

## See Also

- [GitHub Actions Workflow](../.github/workflows/build.yml)
- [Windows Build Guide](../WINDOWS_BUILD.md)
- [Contributing Guide](../CONTRIBUTING.md)
