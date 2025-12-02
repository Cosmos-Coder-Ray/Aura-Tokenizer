# Aura Tokenizer - API Reference

Complete API reference for the Aura Tokenizer library across all supported languages.

## Table of Contents

- [Python API](#python-api)
  - [BPE Tokenizer](#bpe-tokenizer)
  - [WordPiece Tokenizer](#wordpiece-tokenizer)
  - [Unigram Tokenizer](#unigram-tokenizer)
  - [CharLevel Tokenizer](#charlevel-tokenizer)
  - [Generic Tokenizer](#generic-tokenizer)
  - [Encoding](#encoding)
- [Rust API](#rust-api)
- [C++ API](#c-api)

---

## Python API

### BPETokenizer

Byte-Pair Encoding tokenizer implementation.

#### Constructor

```python
BPETokenizer(vocab_path: str | None = None, merges_path: str | None = None)
```

**Parameters:**
- `vocab_path` (optional): Path to vocabulary file
- `merges_path` (optional): Path to merges file

**Example:**
```python
from aura_tokenizer import BPETokenizer

# Create new BPE tokenizer
tokenizer = BPETokenizer()

# Load from existing files
tokenizer = BPETokenizer(vocab_path="vocab.json", merges_path="merges.txt")
```

#### Methods

##### `train(files: List[str], vocab_size: int) -> None`

Train the tokenizer on a corpus.

**Parameters:**
- `files`: List of file paths containing training data
- `vocab_size`: Target vocabulary size

**Example:**
```python
tokenizer.train(["corpus1.txt", "corpus2.txt"], vocab_size=30000)
```

##### `encode(text: str) -> Encoding`

Encode text into token IDs.

**Parameters:**
- `text`: Input text to encode

**Returns:**
- `Encoding` object with `input_ids` and `attention_mask`

**Example:**
```python
encoding = tokenizer.encode("Hello, world!")
print(encoding.input_ids)  # [101, 7592, 1010, 2088, 999, 102]
```

##### `decode(ids: List[int], skip_special_tokens: bool = True) -> str`

Decode token IDs back to text.

**Parameters:**
- `ids`: List of token IDs
- `skip_special_tokens`: Whether to skip special tokens in output

**Returns:**
- Decoded text string

**Example:**
```python
text = tokenizer.decode([101, 7592, 1010, 2088, 999, 102])
print(text)  # "Hello, world!"
```

##### `save(path: str) -> None`

Save tokenizer configuration and vocabulary.

**Parameters:**
- `path`: File path to save tokenizer

**Example:**
```python
tokenizer.save("my_tokenizer.json")
```

##### `get_vocab_size() -> int`

Get the vocabulary size.

**Returns:**
- Integer vocabulary size

##### `set_normalization_form(form: str) -> None`

Set Unicode normalization form.

**Parameters:**
- `form`: One of "NFC", "NFD", "NFKC", "NFKD", "NONE"

**Example:**
```python
tokenizer.set_normalization_form("NFKC")
```

##### `set_strip_accents(strip: bool) -> None`

Enable/disable accent stripping.

**Parameters:**
- `strip`: Whether to strip accents

##### `set_lowercase(lowercase: bool) -> None`

Enable/disable lowercase normalization.

**Parameters:**
- `lowercase`: Whether to convert to lowercase

##### `add_pre_tokenizer_pattern(pattern: str) -> None`

Add a regex pattern for pre-tokenization.

**Parameters:**
- `pattern`: Regex pattern string

**Example:**
```python
tokenizer.add_pre_tokenizer_pattern(r"\w+|[^\w\s]+")
```

##### `create_bert_post_processor(add_special_tokens: bool) -> None`

Create BERT-style post processor.

**Parameters:**
- `add_special_tokens`: Whether to add [CLS] and [SEP] tokens

##### `add_special_tokens(tokens: List[str]) -> None`

Add special tokens to vocabulary.

**Parameters:**
- `tokens`: List of special token strings

---

### WordPieceTokenizer

WordPiece tokenizer (used by BERT).

#### Constructor

```python
WordPieceTokenizer(vocab_path: str | None = None)
```

**Parameters:**
- `vocab_path` (optional): Path to vocabulary file

**Example:**
```python
from aura_tokenizer import WordPieceTokenizer

# Create new WordPiece tokenizer
tokenizer = WordPieceTokenizer()

# Load from existing vocab
tokenizer = WordPieceTokenizer(vocab_path="bert-vocab.txt")
```

#### Methods

All methods from BPETokenizer except `train()` (WordPiece uses pre-built vocabularies).

---

### UnigramTokenizer

Unigram language model tokenizer (used by SentencePiece).

#### Constructor

```python
UnigramTokenizer(vocab_path: str | None = None)
```

**Parameters:**
- `vocab_path` (optional): Path to vocabulary file with scores

**Example:**
```python
from aura_tokenizer import UnigramTokenizer

tokenizer = UnigramTokenizer()
# or load existing
tokenizer = UnigramTokenizer(vocab_path="unigram.model")
```

#### Methods

Same as BPETokenizer, including `train()`.

---

### CharLevelTokenizer

Character-level tokenizer.

#### Constructor

```python
CharLevelTokenizer()
```

**Example:**
```python
from aura_tokenizer import CharLevelTokenizer

tokenizer = CharLevelTokenizer()
```

#### Methods

Includes `train()`, `encode()`, `decode()`, `save()`, `get_vocab_size()`, `set_normalization_form()`.

---

### Generic Tokenizer

Legacy generic tokenizer (backward compatibility).

#### Constructor

```python
Tokenizer(path: str | None = None)
```

**Parameters:**
- `path` (optional): Path to load existing tokenizer

**Example:**
```python
from aura_tokenizer import Tokenizer

tokenizer = Tokenizer()  # Creates BPE tokenizer
# or load
tokenizer = Tokenizer("my_tokenizer.json")
```

#### Methods

All methods from BPETokenizer plus:

##### `token_to_id(token: str) -> int`

Get token ID for a token string.

##### `id_to_token(id: int) -> str`

Get token string for a token ID.

---

### Encoding

Result object from encoding operations.

#### Attributes

- `input_ids: List[int]` - Token IDs
- `attention_mask: List[int]` - Attention mask (1 for real tokens, 0 for padding)

**Example:**
```python
encoding = tokenizer.encode("Hello world")
print(f"IDs: {encoding.input_ids}")
print(f"Mask: {encoding.attention_mask}")
```

---

## Rust API

### BPETokenizer

```rust
use aura_tokenizer::BPETokenizer;

// Create new tokenizer
let mut tokenizer = BPETokenizer::new();

// Load from files
let tokenizer = BPETokenizer::from_files("vocab.json", "merges.txt");

// Train
tokenizer.train_from_files(vec!["corpus.txt".to_string()], 30000);

// Encode
let encoding = tokenizer.encode("Hello, world!");
println!("{:?}", encoding.input_ids);

// Decode
let text = tokenizer.decode(&encoding.input_ids, true);

// Save
tokenizer.save("tokenizer.json");

// Configuration
tokenizer.set_normalization_form("NFKC");
tokenizer.set_lowercase(true);
```

### WordPieceTokenizer

```rust
use aura_tokenizer::WordPieceTokenizer;

let tokenizer = WordPieceTokenizer::new();
// or
let tokenizer = WordPieceTokenizer::from_file("vocab.txt");
```

### UnigramTokenizer

```rust
use aura_tokenizer::UnigramTokenizer;

let mut tokenizer = UnigramTokenizer::new();
tokenizer.train_from_files(vec!["data.txt".to_string()], 30000);
```

### CharLevelTokenizer

```rust
use aura_tokenizer::CharLevelTokenizer;

let mut tokenizer = CharLevelTokenizer::new();
tokenizer.train_from_files(vec!["data.txt".to_string()], 256);
```

### Generic Tokenizer

```rust
use aura_tokenizer::Tokenizer;

let mut tokenizer = Tokenizer::new_bpe();
// or load
let tokenizer = Tokenizer::from_file("tokenizer.json");

// Additional methods
let token_id = tokenizer.token_to_id("hello");
let token = tokenizer.id_to_token(42);
```

---

## C++ API

### TokenizerAdvanced

```cpp
#include "tokenizer_advanced.h"

// Create configuration
TokenizerConfig config;
config.vocab_size = 30000;
config.unk_token = "[UNK]";

// Create model
auto model = std::make_shared<TokenizerModel>();
model->set_algorithm(TokenizationAlgorithm::BPE);

// Create components
auto normalizer = std::make_shared<UnicodeNormalizer>(config);
auto pre_tokenizer = std::make_shared<RegexPreTokenizer>(config);

// Create tokenizer
TokenizerAdvanced tokenizer(config, model, normalizer, pre_tokenizer);

// Train
std::vector<std::string> files = {"corpus.txt"};
tokenizer.train(files, 30000);

// Encode
auto encoding = tokenizer.encode("Hello, world!");
for (int id : encoding.input_ids[0]) {
    std::cout << id << " ";
}

// Decode
std::string text = tokenizer.decode(encoding.input_ids[0], true);

// Save
tokenizer.save("tokenizer.json");
```

---

## Thread Safety

### Python

All Python tokenizer classes use `Arc<Mutex<T>>` internally, making them thread-safe. You can safely use tokenizers across multiple threads:

```python
from concurrent.futures import ThreadPoolExecutor
from aura_tokenizer import BPETokenizer

tokenizer = BPETokenizer()
texts = ["text1", "text2", "text3"]

with ThreadPoolExecutor(max_workers=4) as executor:
    encodings = list(executor.map(tokenizer.encode, texts))
```

### Rust

Rust tokenizer types implement `Send`, allowing them to be moved between threads. Wrap in `Arc<Mutex<T>>` for shared access:

```rust
use std::sync::{Arc, Mutex};
use std::thread;

let tokenizer = Arc::new(Mutex::new(BPETokenizer::new()));
let tokenizer_clone = Arc::clone(&tokenizer);

thread::spawn(move || {
    let enc = tokenizer_clone.lock().unwrap().encode("hello");
});
```

---

## Error Handling

### Python

All methods that can fail return `PyResult` and raise Python exceptions:

```python
try:
    tokenizer.save("/invalid/path/file.json")
except RuntimeError as e:
    print(f"Error: {e}")
```

### Rust

Methods use `panic!` for unrecoverable errors (e.g., uninitialized tokenizer):

```rust
let tokenizer = BPETokenizer::new();
// This will panic if tokenizer.inner is null
let encoding = tokenizer.encode("text");
```

---

## Performance Tips

1. **Batch Processing**: Encode multiple texts in a loop rather than individually
2. **Vocabulary Size**: Larger vocabularies = faster encoding but more memory
3. **Pre-tokenization**: Custom regex patterns can improve speed for specific domains
4. **Thread Safety Overhead**: Python's `Arc<Mutex>` adds overhead; use multiprocessing for CPU-bound tasks

---

## See Also

- [Usage Guide](usage.md)
- [Python Examples](examples.ipynb)
- [Rust Examples](examples_rust.md)
- [Contributing](../CONTRIBUTING.md)
