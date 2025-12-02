# Aura-Tokenizer (Rust)

High-performance tokenizer library with C++ core and Rust bindings.

## Installation

Add this to your `Cargo.toml`:

```toml
[dependencies]
aura-tokenizer = "0.1.0"
```

## Usage

```rust
use aura_tokenizer::Tokenizer;

fn main() {
    // Initialize tokenizer (BPE by default)
    let mut tokenizer = Tokenizer::new_bpe();

    // Train from files
    // tokenizer.train_from_files(vec!["corpus.txt".to_string()], 30000);

    // Configure Normalizer
    tokenizer.set_normalization_form("NFC");
    tokenizer.set_strip_accents(true);
    tokenizer.set_lowercase(true);

    // Add custom pre-tokenizer patterns
    tokenizer.add_pre_tokenizer_pattern(r"\d+");

    // Create BERT Post-Processor (adds [CLS], [SEP])
    tokenizer.create_bert_post_processor(true);

    // Encode text
    let text = "Hello, world!";
    let encoding = tokenizer.encode(text);
    println!("IDs: {:?}", encoding.input_ids);

    // Decode
    let decoded = tokenizer.decode(&encoding.input_ids, true);
    println!("Decoded: {}", decoded);

    // Save
    // tokenizer.save("tokenizer.json");
}
```

## Features

- **Algorithms**: BPE, Unigram, WordPiece, Char-level
- **Training**: Train BPE/Unigram models from files
- **Normalization**: Unicode normalization (NFC/NFD/etc.), accent stripping, lowercasing
- **Pre-tokenization**: Regex-based splitting
- **Post-processing**: BERT-style ([CLS], [SEP])
- **Safety**: Pure Rust FFI bridge to C++ core

## Building

To build locally:

```bash
cargo build
```

This will automatically compile the C++ core and link it.
