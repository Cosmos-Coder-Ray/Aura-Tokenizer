# Aura-Tokenizer

**A high-performance, production-grade tokenizer library with a C++ core and multi-platform support.**

- Supports BPE, Unigram, WordPiece, Char-level, and advanced neural NLP workflows
- Extensible with advanced pre-tokenizers, post-processors, plug-ins, and streaming utilities
- Ready for LLMs, neural networks, and datasets >500GB
- Hugging Face Transformers and Datasets compatible
- Complete WASM/Node.js/browser support
- Pure Rust FFI bridge for maximum safety and performance
- Compatible with Python, Rust, C++, and web platforms

## Features

- **High-Performance C++ Core**
  - Optimized algorithms and data structures
  - Multi-threading support
  - Memory-efficient processing
  - Production-ready error handling

- **Modern Cross-Platform Support**
  - Python via Rust FFI (replacing Cython)
  - Native Rust integration
  - WebAssembly for browsers and Node.js
  - Native C++ library

- **Tokenization Algorithms**
  - BPE (Byte-Pair Encoding)
  - Unigram
  - WordPiece
  - Character-level
  
- **Advanced Processing**
  - Unicode normalization (NFC, NFD, NFKC, NFKD)
  - Extensible pre-tokenizers (Regex patterns)
  - Configurable post-processors (BERT-style)
  
## Installation

### Python
```bash
pip install aura-tokenizer
```

### Rust
```toml
[dependencies]
aura-tokenizer = "0.1.0"
```

## Usage

### Python
```python
from aura_tokenizer import Tokenizer

# Initialize tokenizer (BPE by default)
tokenizer = Tokenizer()

# Train from files
tokenizer.train(["corpus.txt"], vocab_size=30000)

# Configure Normalizer
tokenizer.set_normalization_form("NFC")
tokenizer.set_strip_accents(True)
tokenizer.set_lowercase(True)

# Add custom pre-tokenizer patterns
tokenizer.add_pre_tokenizer_pattern(r"\d+")

# Create BERT Post-Processor (adds [CLS], [SEP])
tokenizer.create_bert_post_processor(True)

# Encode text
encoding = tokenizer.encode("Hello, world!")
print(encoding.input_ids)

# Decode
print(tokenizer.decode(encoding.input_ids))

# Save
tokenizer.save("tokenizer.json")
```

### Rust
```rust
use aura_tokenizer::Tokenizer;

// Initialize
let mut tokenizer = Tokenizer::new_bpe();

// Train from files
tokenizer.train_from_files(vec!["corpus.txt".to_string()], 30000);

// Configure
tokenizer.set_normalization_form("NFC");
tokenizer.set_strip_accents(true);
tokenizer.set_lowercase(true);

// Add custom pre-tokenizer patterns
tokenizer.add_pre_tokenizer_pattern(r"\d+");

// Create BERT Post-Processor
tokenizer.create_bert_post_processor(true);

// Encode
let encoding = tokenizer.encode("Hello, world!");
println!("{:?}", encoding.input_ids);

// Decode
let decoded = tokenizer.decode(&encoding.input_ids, true);
println!("{}", decoded);
```

## Documentation

For complete documentation, see:
- [Python API Reference](docs/api.rst)
- [Rust API Reference](docs/rust_api.md)
- [C++ API Reference](docs/cpp_api.md)

## Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.