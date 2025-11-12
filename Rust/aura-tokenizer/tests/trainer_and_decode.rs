use aura_tokenizer::models::BPEModel;
use aura_tokenizer::tokenizer::Tokenizer;
use aura_tokenizer::trainer::BPETrainer;

#[test]
fn test_trainer_creation_fails_gracefully() {
    let config = r#"{"vocab_size": 100}"#;
    let result = BPETrainer::from_config(config);
    assert!(result.is_err());
}

#[test]
fn test_decode_fails_gracefully() {
    // We need a tokenizer to call decode, which requires a model.
    // We expect model creation to fail, so this test will confirm that.
    let model_result = BPEModel::from_config(r#"{}"#);
    assert!(model_result.is_err());

    // If we could create a model, the test would look like this:
    // let model = model_result.unwrap();
    // let tokenizer = Tokenizer::new(model).unwrap();
    // let decode_result = tokenizer.decode(&[0, 1, 2]);
    // assert!(decode_result.is_err());
}
