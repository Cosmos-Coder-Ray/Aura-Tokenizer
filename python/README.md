# Aura-Tokenizer (Python)

Enterprise-grade Tokenizer with C++ Core and Rust/Python Bindings.

## Installation

```bash
pip install aura-tokenizer
```

## Usage

```python
from aura_tokenizer import Tokenizer

# Initialize
tokenizer = Tokenizer()

# Train
tokenizer.train(["corpus.txt"], 30000)

# Configure
tokenizer.set_normalization_form("NFC")
tokenizer.set_lowercase(True)
tokenizer.create_bert_post_processor(True)

# Encode
encoding = tokenizer.encode("Hello World")
print(encoding.input_ids)
```

See [GitHub Repository](https://github.com/username/aura-tokenizer) for more details.
