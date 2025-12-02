# Aura-Tokenizer Rust Examples

## Basic Usage

```rust
use aura_tokenizer::Tokenizer;

fn main() {
    // Initialize
    let mut tokenizer = Tokenizer::new_bpe();

    // Train
    tokenizer.train_from_files(vec!["corpus.txt".to_string()], 30000);

    // Encode
    let encoding = tokenizer.encode("Hello world");
    println!("{:?}", encoding.input_ids);
}
```

## Advanced Configuration

```rust
use aura_tokenizer::Tokenizer;

fn main() {
    let mut tokenizer = Tokenizer::new_bpe();

    // Normalization
    tokenizer.set_normalization_form("NFC");
    tokenizer.set_lowercase(true);
    tokenizer.set_strip_accents(true);

    // Pre-tokenization
    tokenizer.add_pre_tokenizer_pattern(r"\d+");

    // Post-processing
    tokenizer.create_bert_post_processor(true);

    // Encode
    let encoding = tokenizer.encode("Hello World");
    println!("{:?}", encoding.input_ids);
}
```
