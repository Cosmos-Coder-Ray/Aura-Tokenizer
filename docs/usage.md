# Aura-Tokenizer Python Usage Guide

## Installation

```bash
pip install aura-tokenizer
```

## Basic Usage

### Initialization

```python
from aura_tokenizer import Tokenizer

# Initialize a BPE tokenizer (default)
tokenizer = Tokenizer()
```

### Training

Train a new model from a list of files:

```python
files = ["corpus.txt"]
vocab_size = 30000
tokenizer.train(files, vocab_size)
```

### Encoding & Decoding

```python
text = "Hello, world!"
encoding = tokenizer.encode(text)

print(f"Input IDs: {encoding.input_ids}")
print(f"Attention Mask: {encoding.attention_mask}")

decoded = tokenizer.decode(encoding.input_ids)
print(f"Decoded: {decoded}")
```

## Advanced Configuration

### Normalization

Configure Unicode normalization and text cleaning:

```python
# Set normalization form (NFC, NFD, NFKC, NFKD)
tokenizer.set_normalization_form("NFC")

# Strip accents (e.g., 'é' -> 'e')
tokenizer.set_strip_accents(True)

# Lowercase all input
tokenizer.set_lowercase(True)
```

### Pre-tokenization

Add custom regex patterns to split text before tokenization:

```python
# Split on digits
tokenizer.add_pre_tokenizer_pattern(r"\d+")
```

### Post-processing

Add BERT-style special tokens ([CLS], [SEP]):

```python
# True to add special tokens
tokenizer.create_bert_post_processor(True)
```

### Special Tokens

Manually add special tokens:

```python
tokenizer.add_special_tokens(["[MY_TOKEN]", "[ANOTHER]"])
```

## Saving and Loading

```python
# Save to JSON
tokenizer.save("tokenizer.json")

# Load from JSON (Note: Loading logic depends on implementation, currently creates new instance)
tokenizer = Tokenizer("tokenizer.json")
```
