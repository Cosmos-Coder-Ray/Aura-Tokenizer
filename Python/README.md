# Aura Tokenizer Python Bindings

This directory contains the Python bindings for the `aura-tokenizer` library, providing a high-performance and user-friendly API for Python users.

The architecture is `Python -> Rust (PyO3) -> aura-tokenizer (safe Rust) -> C++`.

## Building and Development

This project uses `maturin` to build the Rust-powered Python module.

### Prerequisites

- Python 3.7+
- A virtual environment
- Rust (via `rustup`)

### Installation

1.  **Create a virtual environment:**
    ```bash
    python -m venv .venv
    ```

2.  **Install the package in editable mode:**
    This command will compile the Rust extension and install it into your virtual environment.
    ```bash
    maturin develop
    ```

### Running Tests

Tests are located in the `tests/` directory and can be run with `pytest`.

```bash
pytest
```
