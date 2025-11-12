//! A safe, ergonomic, and high-performance tokenization library for Rust.

pub mod error;
pub mod hf_compat;
pub mod models;
pub mod tokenizer;
pub mod trainer;

#[cfg(test)]
mod tests {
    use super::models::BPEModel;

    #[test]
    fn model_creation_and_drop() {
        // This test will fail at runtime because the underlying C++ library is a placeholder.
        // However, it serves as a crucial compile-time check to ensure that:
        // 1. The `BPEModel::from_config` function can be called.
        // 2. The `BPEModel` struct can be instantiated.
        // 3. The `Drop` trait is implemented and will be called when `model` goes out of scope.
        //
        // We expect this to panic or error out when run, but successfully compiling is the key.
        let config = r#"{"model_type": "bpe"}"#;
        let _model = BPEModel::from_config(config);
        // `_model` is dropped here, and our `Drop` implementation will be called.
    }
}
