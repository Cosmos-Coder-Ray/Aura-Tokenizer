use cxx::UniquePtr;

#[cxx::bridge(namespace = "auratokenizer")]
pub mod ffi {
    // Shared structs between Rust and C++
    struct RustTokenizerConfig {
        unk_token: String,
        pad_token: String,
        bos_token: String,
        eos_token: String,
        mask_token: String,
        sep_token: String,
        cls_token: String,
    }

    unsafe extern "C++" {
        include!("tokenizer_advanced.h");
        include!("tokenizer_config.h");
        include!("bridge.h");

        type TokenizerAdvanced;
        type TokenizerConfig;
        type BatchEncoding;

        // Factory functions
        fn create_bpe_tokenizer(
            config: &RustTokenizerConfig,
            vocab_path: &String,
            merges_path: &String,
        ) -> UniquePtr<TokenizerAdvanced>;
        fn create_wordpiece_tokenizer(
            config: &RustTokenizerConfig,
            vocab_path: &String,
        ) -> UniquePtr<TokenizerAdvanced>;
        fn create_unigram_tokenizer(
            config: &RustTokenizerConfig,
            vocab_path: &String,
        ) -> UniquePtr<TokenizerAdvanced>;
        fn create_charlevel_tokenizer(config: &RustTokenizerConfig)
            -> UniquePtr<TokenizerAdvanced>;
        fn load_tokenizer(path: &String) -> UniquePtr<TokenizerAdvanced>;

        // Vocabulary & Special Tokens
        fn get_vocab_size(self: &TokenizerAdvanced) -> usize;

        // Bridge helper functions
        fn save(tokenizer: &TokenizerAdvanced, path: &String);
        fn encode(tokenizer: &TokenizerAdvanced, text: &String) -> UniquePtr<BatchEncoding>;
        fn decode(
            tokenizer: &TokenizerAdvanced,
            ids: &Vec<i32>,
            skip_special_tokens: bool,
        ) -> String;
        fn token_to_id(tokenizer: &TokenizerAdvanced, token: &String) -> i32;
        fn id_to_token(tokenizer: &TokenizerAdvanced, id: i32) -> String;
        fn add_special_tokens(tokenizer: Pin<&mut TokenizerAdvanced>, tokens: &Vec<String>);
        fn train_from_files(
            tokenizer: Pin<&mut TokenizerAdvanced>,
            files: &Vec<String>,
            vocab_size: usize,
        );

        // Configuration methods
        fn set_normalization_form(tokenizer: Pin<&mut TokenizerAdvanced>, form: &String);
        fn set_strip_accents(tokenizer: Pin<&mut TokenizerAdvanced>, strip: bool);
        fn set_lowercase(tokenizer: Pin<&mut TokenizerAdvanced>, lowercase: bool);
        fn add_pre_tokenizer_pattern(tokenizer: Pin<&mut TokenizerAdvanced>, pattern: &String);
        fn create_bert_post_processor(
            tokenizer: Pin<&mut TokenizerAdvanced>,
            add_special_tokens: bool,
        );

        // BatchEncoding helpers
        fn get_encoding_ids(encoding: &BatchEncoding, index: usize) -> Vec<i32>;
        fn get_encoding_mask(encoding: &BatchEncoding, index: usize) -> Vec<i32>;
    }
}
