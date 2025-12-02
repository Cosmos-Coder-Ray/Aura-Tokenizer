use aura_tokenizer::Tokenizer;
use std::fs::File;
use std::io::Write;
use tempfile::NamedTempFile;

#[test]
fn test_full_pipeline() {
    // 1. Create a dummy corpus
    let mut file = NamedTempFile::new().unwrap();
    writeln!(
        file,
        "Hello world! This is a test corpus for Aura Tokenizer."
    )
    .unwrap();
    let path = file.path().to_str().unwrap().to_string();

    // 2. Initialize Tokenizer
    let mut tokenizer = Tokenizer::new_bpe();

    // 3. Train
    tokenizer.train_from_files(vec![path], 1000);

    // 4. Configure
    tokenizer.set_normalization_form("NFC");
    tokenizer.set_lowercase(true);
    tokenizer.create_bert_post_processor(true);

    // 5. Encode
    let text = "Hello World";
    let encoding = tokenizer.encode(text);

    // Verify
    assert!(!encoding.input_ids.is_empty());
    println!("IDs: {:?}", encoding.input_ids);

    // 6. Decode
    let decoded = tokenizer.decode(&encoding.input_ids, true);
    // Note: Decoded might be lowercased due to configuration
    assert!(decoded.to_lowercase().contains("hello"));
}
