# Contributing to Aura Tokenizer

Thank you for your interest in contributing to Aura Tokenizer! This document provides guidelines and instructions for contributing.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [Making Changes](#making-changes)
- [Testing](#testing)
- [Submitting Changes](#submitting-changes)
- [Code Style](#code-style)
- [Architecture Overview](#architecture-overview)

---

## Code of Conduct

We are committed to providing a welcoming and inclusive environment. Please:

- Be respectful and considerate
- Welcome newcomers and help them get started
- Focus on what is best for the community
- Show empathy towards other community members

---

## Getting Started

### Prerequisites

- **C++ Compiler**: MSVC (Windows), GCC/Clang (Linux/Mac)
- **Rust**: 1.70+ (`rustup` recommended)
- **Python**: 3.8+
- **CMake**: 3.15+ (for C++ builds)
- **Maturin**: For Python package building

### Building from Source

```bash
# Clone the repository
git clone https://github.com/yourusername/aura-tokenizer.git
cd aura-tokenizer

# Build Rust crate
cd rust
cargo build --release

# Build Python bindings
cd ../python
maturin develop

# Run tests
cd ../rust
cargo test
cd ../python
pytest
```

---

## Development Setup

### IDE Configuration

We recommend:
- **VS Code** with rust-analyzer extension
- **CLion** or **Visual Studio** for C++ development
- **PyCharm** for Python development

### Project Structure

```
Aura-Tokenizer/
├── Aura-Tokenizer/       # C++ core library
│   ├── include/          # Public headers
│   └── src/              # Implementation
├── rust/                 # Rust bindings
│   ├── src/              # Rust wrappers
│   ├── bridge/           # C++ FFI bridge
│   └── tests/            # Integration tests
├── python/               # Python bindings
│   ├── src/              # PyO3 bindings
│   └── tests/            # Python tests
└── docs/                 # Documentation
```

---

## Making Changes

### Finding Issues to Work On

- Check [Issues](https://github.com/yourusername/aura-tokenizer/issues)
- Look for `good first issue` or `help wanted` labels
- Ask in discussions if you're unsure where to start

### Creating a Branch

```bash
git checkout -b feature/your-feature-name
# or
git checkout -b fix/issue-number-description
```

### Commit Messages

Follow [Conventional Commits](https://www.conventionalcommits.org/):

```
feat: add support for custom merges in BPE
fix: resolve memory leak in UnigramTokenizer
docs: update API reference for WordPiece
test: add integration tests for CharLevel
refactor: simplify FFI bridge code
```

---

## Testing

### Running Tests

**Rust:**
```bash
cd rust
cargo test
cargo test --release  # For performance tests
```

**Python:**
```bash
cd python
pytest
pytest -v  # Verbose output
pytest tests/test_specific.py  # Specific test file
```

**C++:**
```bash
cd Aura-Tokenizer
mkdir build && cd build
cmake ..
make test
```

### Writing Tests

#### Rust Tests

```rust
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_bpe_encoding() {
        let mut tokenizer = BPETokenizer::new();
        // ... test logic
    }
}
```

#### Python Tests

```python
import pytest
from aura_tokenizer import BPETokenizer

def test_bpe_encoding():
    tokenizer = BPETokenizer()
    encoding = tokenizer.encode("test")
    assert len(encoding.input_ids) > 0
```

### Test Coverage

- Aim for >80% code coverage
- Include edge cases and error conditions
- Test thread safety for concurrent operations

---

## Submitting Changes

### Pull Request Process

1. **Update Documentation**: Ensure README, API docs, and inline comments are updated
2. **Add Tests**: All new features must include tests
3. **Run Linters**: 
   ```bash
   # Rust
   cargo clippy
   cargo fmt --check
   
   # Python
   ruff check
   black --check .
   ```

4. **Create Pull Request**:
   - Use a clear, descriptive title
   - Reference related issues
   - Describe what changed and why
   - Include screenshots for UI changes

5. **Code Review**: Address reviewer feedback promptly

### PR Template

```markdown
## Description
Brief description of changes

## Related Issues
Fixes #123

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Checklist
- [ ] Tests added/updated
- [ ] Documentation updated
- [ ] Code follows style guidelines
- [ ] All tests pass
```

---

## Code Style

### C++

- Follow **Google C++ Style Guide**
- Use **snake_case** for functions and variables
- Use **PascalCase** for classes
- Include guards: `#pragma once`
- Pointer alignment: `Type* ptr`

```cpp
class BPETokenizer {
public:
    void train_from_files(const std::vector<std::string>& files, size_t vocab_size);
    
private:
    std::shared_ptr<Vocab> vocab_;
};
```

### Rust

- Follow **Rust API Guidelines**
- Use `cargo fmt` for formatting
- Use `cargo clippy` for linting
- Document public APIs with `///`

```rust
/// Encodes text into token IDs.
///
/// # Arguments
/// * `text` - The input text to encode
///
/// # Returns
/// An `Encoding` containing token IDs and attention mask
pub fn encode(&self, text: &str) -> Encoding {
    // implementation
}
```

### Python

- Follow **PEP 8**
- Use **type hints** for all public APIs
- Use `ruff` for linting
- Use `black` for formatting

```python
def encode(self, text: str) -> Encoding:
    """Encode text into token IDs.
    
    Args:
        text: Input text to encode
        
    Returns:
        Encoding object with token IDs and attention mask
    """
    pass
```

---

## Architecture Overview

### FFI Layer

```
Python (PyO3)
    ↓
Rust (aura-tokenizer crate)
    ↓
C++ Bridge (bridge.cpp)
    ↓
C++ Core (Aura-Tokenizer/)
```

### Key Design Decisions

1. **Thread Safety**: Python uses `Arc<Mutex<T>>` for thread-safe access
2. **Memory Management**: C++ uses smart pointers, Rust uses `UniquePtr` via cxx
3. **Error Handling**: C++ exceptions → Rust panics → Python exceptions

### Adding New Features

1. **C++ Core**: Implement in `Aura-Tokenizer/src/`
2. **FFI Bridge**: Add bridge functions in `rust/bridge/bridge.cpp`
3. **Rust Wrapper**: Add safe wrappers in `rust/src/tokenizer.rs`
4. **Python Bindings**: Expose via PyO3 in `python/src/lib.rs`
5. **Tests**: Add tests at each layer
6. **Documentation**: Update API reference and examples

---

## Release Process

1. Update version in `Cargo.toml` and `pyproject.toml`
2. Update CHANGELOG.md
3. Create git tag: `git tag -a v1.0.0 -m "Release 1.0.0"`
4. Build and publish:
   ```bash
   # Rust crate
   cargo publish
   
   # Python package
   maturin build --release
   maturin upload target/wheels/*
   ```

---

## Getting Help

- **Discussions**: https://github.com/yourusername/aura-tokenizer/discussions
- **Issues**: https://github.com/yourusername/aura-tokenizer/issues
- **Email**: maintainers@aura-tokenizer.dev

---

## Recognition

Contributors will be:
- Listed in AUTHORS.md
- Mentioned in release notes
- Credited in documentation

Thank you for contributing to Aura Tokenizer! 🚀
