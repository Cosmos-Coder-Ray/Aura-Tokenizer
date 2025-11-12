# Aura Tokenizer (aura-tokenizer)

This crate provides a safe, ergonomic, and high-performance Rust API for the `auratokenizer` library.

It is built on top of the unsafe `aura-tokenizer-sys` crate and provides a high-level abstraction that is memory-safe and easy to use.

## Features

- **Safe:** All C-API pointers are wrapped in structs that manage their lifecycle, preventing memory leaks via the `Drop` trait.
- **Ergonomic:** C-API error codes are converted into a proper Rust `Result<T, AuraError>`.
- **Innovative:** Includes a compatibility layer for the Hugging Face `tokenizers` ecosystem, with a rich `Encoding` object and support for `tokenizer.json` (via `serde`).

## Building

This crate can be built and tested using standard Cargo commands:

```bash
# Build the library
cargo build

# Run the tests
cargo test
```
