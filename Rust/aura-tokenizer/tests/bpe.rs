use aura_tokenizer::models::BPEModel;

#[test]
fn test_bpe_model_creation_fails_gracefully() {
    // This integration test validates that the safe Rust wrapper correctly
    // handles the error (a null pointer) returned by the placeholder C++ library.

    let config = r#"{"model_type": "bpe"}"#;
    let result = BPEModel::from_config(config);

    // We expect an error because the C++ library is a placeholder.
    // The key is that this returns an Err, and doesn't panic or segfault.
    assert!(result.is_err());

    // We can also check that the error is the one we expect.
    let err = result.unwrap_err();
    assert!(matches!(
        err,
        aura_tokenizer::error::AuraError::InvalidConfig(_)
    ));
}
