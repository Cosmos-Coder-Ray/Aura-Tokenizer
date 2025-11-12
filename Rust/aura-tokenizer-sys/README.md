# Aura Tokenizer FFI (aura-tokenizer-sys)

This crate provides the raw, unsafe FFI bindings to the C++ `auratokenizer` library.

It is the foundational layer that connects Rust to the C++ core. This crate is not intended for direct use. The safe, ergonomic API is provided by the `aura-tokenizer` crate.

## Building

This crate is built automatically as a dependency of `aura-tokenizer`. It uses a `build.rs` script to:
1.  Locate the pre-compiled `auratokenizer` C++ library.
2.  Use `bindgen` to generate Rust FFI bindings from the `auratokenizer_c_api.h` header.
3.  Link the C++ library to the Rust build.
