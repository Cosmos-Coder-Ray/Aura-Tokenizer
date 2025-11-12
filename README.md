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
  - Unicode normalization
  - Extensible pre-tokenizers
  - Configurable post-processors
  - Template-based processing
  
- **Industrial-Strength Features**
  - Streaming support for huge datasets (500GB+)
  - Efficient batch processing
  - Memory-mapped file support
  - Progress tracking for long operations
  - Comprehensive error handling
  
- **Developer Experience**
  - Type-safe APIs across all platforms
  - Comprehensive documentation
  - Integration examples
  - Automated tests

## Installation

### Python
```bash
pip install aura-tokenizer
```

### Rust
```toml
[dependencies]
aura-tokenizer = "1.0"
```

### C++
```bash
git clone https://github.com/username/aura-tokenizer.git
cd aura-tokenizer
mkdir build && cd build
cmake ..
make install
```

### WebAssembly (Node.js/Browser)
```bash
npm install aura-tokenizer
# or
yarn add aura-tokenizer
```

## Usage

### Python
```python
from aura_tokenizer import BPETokenizer

# Initialize tokenizer
tokenizer = BPETokenizer.from_config({
    "model_type": "bpe",
    "vocab_size": 30000,
    "special_tokens": ["[UNK]", "[PAD]", "[CLS]", "[SEP]"]
})

# Encode text
text = "Hello, world!"
tokens = tokenizer.encode(text)
ids = tokenizer.encode_to_ids(text)

# Decode back to text
decoded = tokenizer.decode(tokens)
```

### Rust
```rust
use aura_tokenizer::{BPETokenizer, TokenizerConfig};

// Initialize tokenizer
let config = TokenizerConfig::new()
    .model_type("bpe")
    .vocab_size(30000)
    .special_tokens(vec!["[UNK]", "[PAD]", "[CLS]", "[SEP]"]);

let tokenizer = BPETokenizer::from_config(&config)?;

// Encode text
let text = "Hello, world!";
let tokens = tokenizer.encode(text)?;
let ids = tokenizer.encode_to_ids(text)?;

// Decode back to text
let decoded = tokenizer.decode(&tokens)?;
```

### WebAssembly (TypeScript)
```typescript
import AuraTokenizerInit from 'aura-tokenizer';

async function main() {
    const module = await AuraTokenizerInit();
    const tokenizer = new module.Tokenizer(JSON.stringify({
        modelType: "bpe",
        vocabSize: 30000,
        specialTokens: ["[UNK]", "[PAD]", "[CLS]", "[SEP]"]
    }));

    const text = "Hello, world!";
    const tokens = tokenizer.encode(text);
    const ids = tokenizer.encodeToIds(text);
    const decoded = tokenizer.decode(tokens);
}
```

## Documentation

For complete documentation, see:
- [Python API Reference](docs/api.rst)
- [Rust API Reference](docs/rust_api.md)
- [C++ API Reference](docs/cpp_api.md)
- [WebAssembly API Reference](docs/wasm.md)
- [Advanced Features Guide](docs/advanced.md)
- [Example Projects](docs/examples/)

## Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
- **Offset/alignment utilities**: For NER, QA, and span mapping
- **Incremental/streaming training**: Train on data too large for memory
- **Serialization**: Hugging Face JSON and fast binary formats
- **Plug-in system**: Register custom pre-tokenizers, post-processors, etc.
- **Model-specific features**: Auto templates, padding, truncation, attention masks
- **WASM/Node.js/browser stubs**: Roadmap for web/JS support
- **Hugging Face integration**: Transformers and Datasets compatible
- **Comprehensive tests and documentation**

---

## Installation

```bash
pip install aura-tokenizer
```

Or for development:

```bash
pip install -e .
# Or build Cython/C++ extension in-place
python setup.py build_ext --inplace
```

---

## Quickstart

```python
from aura_tokenizer import BPETokenizer
bpe = BPETokenizer()
bpe.train(["Hello world!"], vocab_size=100)
ids = bpe.encode("Hello world!")
print(ids)
print(bpe.decode(ids))
```

---

## Advanced Usage

### Advanced Pre-tokenizers
```python
from aura_tokenizer.pre_tokenizers import ByteLevelPreTokenizer, MetaspacePreTokenizer, RegexPreTokenizer, DigitPreTokenizer, SplitPreTokenizer, PreTokenizerChain
pt = ByteLevelPreTokenizer()
tokens = pt.pre_tokenize("Hello world!")
print(tokens)
# Chain example:
chain = PreTokenizerChain([RegexPreTokenizer(), DigitPreTokenizer()])
print(chain.pre_tokenize("abc 123!"))
```

### Advanced Post-processors
```python
from aura_tokenizer.post_processors import TemplatePostProcessor, ByteLevelPostProcessor, PostProcessorChain
pp = TemplatePostProcessor('[CLS] $0 [SEP]')
print(pp.post_process(["hello", "world"]))
# Chain example:
chain = PostProcessorChain([TemplatePostProcessor('[CLS] $0 [SEP]'), ByteLevelPostProcessor()])
print(chain.post_process(["h", "e", "l", "l", "o"]))
```

### Streaming & Batch Utilities
```python
from aura_tokenizer.streaming import batch_encode_stream, batch_decode_stream, mmap_file_iterator
for batch in batch_encode_stream(bpe, mmap_file_iterator('big.txt'), batch_size=1024):
    print(batch)
```

### Offset/Alignment Utilities
```python
from aura_tokenizer.offsets import token_to_char_offsets, align_tokens_to_words, batch_offset_mapping
offsets = token_to_char_offsets("Hello world!", ["Hello", "world", "!"])
print(offsets)
```

### Incremental/Streaming Training
```python
from aura_tokenizer.streaming_train import streaming_trainer
streaming_trainer(bpe, mmap_file_iterator('big.txt'), vocab_size=10000, chunk_size=100000)
```

### Serialization/Interoperability
```python
from aura_tokenizer.serialization import save_tokenizer_json, load_tokenizer_json, save_tokenizer_binary, load_tokenizer_binary
save_tokenizer_json(bpe, 'bpe.json')
bpe2 = load_tokenizer_json(BPETokenizer, 'bpe.json')
save_tokenizer_binary(bpe, 'bpe.pkl')
bpe3 = load_tokenizer_binary('bpe.pkl')
```

### Plug-in System
```python
from aura_tokenizer.plugins import plugin_registry
plugin_registry.register_plugin('my_pre', MyCustomPreTokenizer)
print(plugin_registry.list_plugins())
```

### Model-Specific Features
```python
from aura_tokenizer.model_utils import AutoTemplateHandler, pad_sequences, truncate_sequences, generate_attention_mask
handler = AutoTemplateHandler('bert')
tokens = handler.apply_template(["hello", "world"])
print(tokens)
ids = [[1,2,3], [4,5]]
padded = pad_sequences(ids, pad_id=0)
truncated = truncate_sequences(ids, max_length=2)
mask = generate_attention_mask(padded, pad_id=0)
print(padded, truncated, mask)
```

### WASM/Node.js/Browser Stubs
```python
from aura_tokenizer.wasm_stub import WASMTokenizerStub, NodeJSTokenizerStub
# All methods raise NotImplementedError (see docs/wasm.md)
```

### Hugging Face Transformers & Datasets Integration
```python
from aura_tokenizer.hf_integration import AuraTokenizerWrapper, dataset_map_fn
wrapper = AuraTokenizerWrapper(bpe)
print(wrapper("Hello world!", padding=True, truncation=True, max_length=8, return_attention_mask=True))
# For datasets:
# dataset = load_dataset(...)
# dataset = dataset.map(lambda ex: dataset_map_fn(ex, wrapper))
```

---

## Development & Contributing

- Build extension: `python setup.py build_ext --inplace`
- Run tests: `pytest tests/`
- Build docs: `cd docs && make html`
- Lint: `flake8 aura_tokenizer/ tests/`
- Type check: `mypy aura_tokenizer/`

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

---

## License

Aura-Tokenizer is licensed under the Apache 2.0 License. See [LICENSE](LICENSE) for details.

---

## Documentation

- [Full API Reference](docs/api.html)
- [Usage Guide](docs/usage.md)
- [Advanced Guide](docs/advanced.html)
- [WASM/Node.js Roadmap](docs/wasm.md)

---