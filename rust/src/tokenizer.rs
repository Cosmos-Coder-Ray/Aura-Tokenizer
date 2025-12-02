use crate::ffi;
use cxx::UniquePtr;

#[derive(Debug, Clone)]
pub struct Encoding {
    pub input_ids: Vec<i32>,
    pub attention_mask: Vec<i32>,
}

fn default_config() -> ffi::ffi::RustTokenizerConfig {
    ffi::ffi::RustTokenizerConfig {
        unk_token: "[UNK]".to_string(),
        pad_token: "[PAD]".to_string(),
        bos_token: "[BOS]".to_string(),
        eos_token: "[EOS]".to_string(),
        mask_token: "[MASK]".to_string(),
        sep_token: "[SEP]".to_string(),
        cls_token: "[CLS]".to_string(),
    }
}

// ============= BPE Tokenizer =============
pub struct BPETokenizer {
    inner: UniquePtr<ffi::ffi::TokenizerAdvanced>,
}

impl BPETokenizer {
    pub fn new() -> Self {
        let config = default_config();
        let inner = ffi::ffi::create_bpe_tokenizer(&config, &"".to_string(), &"".to_string());
        BPETokenizer { inner }
    }

    pub fn from_files(vocab_path: &str, merges_path: &str) -> Self {
        let config = default_config();
        let inner = ffi::ffi::create_bpe_tokenizer(
            &config,
            &vocab_path.to_string(),
            &merges_path.to_string(),
        );
        BPETokenizer { inner }
    }

    pub fn train_from_files(&mut self, files: Vec<String>, vocab_size: usize) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::train_from_files(self.inner.pin_mut(), &files, vocab_size);
    }

    pub fn encode(&self, text: &str) -> Encoding {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        let batch_encoding_ptr = ffi::ffi::encode(&self.inner, &text.to_string());
        let ids = ffi::ffi::get_encoding_ids(&batch_encoding_ptr, 0);
        let attention_mask = ffi::ffi::get_encoding_mask(&batch_encoding_ptr, 0);
        Encoding {
            input_ids: ids,
            attention_mask,
        }
    }

    pub fn decode(&self, ids: &[i32], skip_special_tokens: bool) -> String {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::decode(&self.inner, &ids.to_vec(), skip_special_tokens)
    }

    pub fn save(&self, path: &str) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::save(&self.inner, &path.to_string());
    }

    pub fn get_vocab_size(&self) -> usize {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        self.inner.get_vocab_size()
    }

    pub fn set_normalization_form(&mut self, form: &str) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::set_normalization_form(self.inner.pin_mut(), &form.to_string());
    }

    pub fn set_strip_accents(&mut self, strip: bool) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::set_strip_accents(self.inner.pin_mut(), strip);
    }

    pub fn set_lowercase(&mut self, lowercase: bool) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::set_lowercase(self.inner.pin_mut(), lowercase);
    }

    pub fn add_pre_tokenizer_pattern(&mut self, pattern: &str) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::add_pre_tokenizer_pattern(self.inner.pin_mut(), &pattern.to_string());
    }

    pub fn create_bert_post_processor(&mut self, add_special_tokens: bool) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::create_bert_post_processor(self.inner.pin_mut(), add_special_tokens);
    }

    pub fn add_special_tokens(&mut self, tokens: Vec<String>) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::add_special_tokens(self.inner.pin_mut(), &tokens);
    }
}

impl Default for BPETokenizer {
    fn default() -> Self {
        Self::new()
    }
}

// ============= WordPiece Tokenizer =============
pub struct WordPieceTokenizer {
    inner: UniquePtr<ffi::ffi::TokenizerAdvanced>,
}

impl WordPieceTokenizer {
    pub fn new() -> Self {
        let config = default_config();
        let inner = ffi::ffi::create_wordpiece_tokenizer(&config, &"".to_string());
        WordPieceTokenizer { inner }
    }

    pub fn from_file(vocab_path: &str) -> Self {
        let config = default_config();
        let inner = ffi::ffi::create_wordpiece_tokenizer(&config, &vocab_path.to_string());
        WordPieceTokenizer { inner }
    }

    pub fn encode(&self, text: &str) -> Encoding {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        let batch_encoding_ptr = ffi::ffi::encode(&self.inner, &text.to_string());
        let ids = ffi::ffi::get_encoding_ids(&batch_encoding_ptr, 0);
        let attention_mask = ffi::ffi::get_encoding_mask(&batch_encoding_ptr, 0);
        Encoding {
            input_ids: ids,
            attention_mask,
        }
    }

    pub fn decode(&self, ids: &[i32], skip_special_tokens: bool) -> String {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::decode(&self.inner, &ids.to_vec(), skip_special_tokens)
    }

    pub fn save(&self, path: &str) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::save(&self.inner, &path.to_string());
    }

    pub fn get_vocab_size(&self) -> usize {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        self.inner.get_vocab_size()
    }

    pub fn set_normalization_form(&mut self, form: &str) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::set_normalization_form(self.inner.pin_mut(), &form.to_string());
    }

    pub fn set_lowercase(&mut self, lowercase: bool) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::set_lowercase(self.inner.pin_mut(), lowercase);
    }

    pub fn create_bert_post_processor(&mut self, add_special_tokens: bool) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::create_bert_post_processor(self.inner.pin_mut(), add_special_tokens);
    }
}

impl Default for WordPieceTokenizer {
    fn default() -> Self {
        Self::new()
    }
}

// ============= Unigram Tokenizer =============
pub struct UnigramTokenizer {
    inner: UniquePtr<ffi::ffi::TokenizerAdvanced>,
}

impl UnigramTokenizer {
    pub fn new() -> Self {
        let config = default_config();
        let inner = ffi::ffi::create_unigram_tokenizer(&config, &"".to_string());
        UnigramTokenizer { inner }
    }

    pub fn from_file(vocab_path: &str) -> Self {
        let config = default_config();
        let inner = ffi::ffi::create_unigram_tokenizer(&config, &vocab_path.to_string());
        UnigramTokenizer { inner }
    }

    pub fn train_from_files(&mut self, files: Vec<String>, vocab_size: usize) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::train_from_files(self.inner.pin_mut(), &files, vocab_size);
    }

    pub fn encode(&self, text: &str) -> Encoding {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        let batch_encoding_ptr = ffi::ffi::encode(&self.inner, &text.to_string());
        let ids = ffi::ffi::get_encoding_ids(&batch_encoding_ptr, 0);
        let attention_mask = ffi::ffi::get_encoding_mask(&batch_encoding_ptr, 0);
        Encoding {
            input_ids: ids,
            attention_mask,
        }
    }

    pub fn decode(&self, ids: &[i32], skip_special_tokens: bool) -> String {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::decode(&self.inner, &ids.to_vec(), skip_special_tokens)
    }

    pub fn save(&self, path: &str) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::save(&self.inner, &path.to_string());
    }

    pub fn get_vocab_size(&self) -> usize {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        self.inner.get_vocab_size()
    }

    pub fn set_normalization_form(&mut self, form: &str) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::set_normalization_form(self.inner.pin_mut(), &form.to_string());
    }

    pub fn set_lowercase(&mut self, lowercase: bool) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::set_lowercase(self.inner.pin_mut(), lowercase);
    }
}

impl Default for UnigramTokenizer {
    fn default() -> Self {
        Self::new()
    }
}

// ============= CharLevel Tokenizer =============
pub struct CharLevelTokenizer {
    inner: UniquePtr<ffi::ffi::TokenizerAdvanced>,
}

impl CharLevelTokenizer {
    pub fn new() -> Self {
        let config = default_config();
        let inner = ffi::ffi::create_charlevel_tokenizer(&config);
        CharLevelTokenizer { inner }
    }

    pub fn train_from_files(&mut self, files: Vec<String>, vocab_size: usize) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::train_from_files(self.inner.pin_mut(), &files, vocab_size);
    }

    pub fn encode(&self, text: &str) -> Encoding {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        let batch_encoding_ptr = ffi::ffi::encode(&self.inner, &text.to_string());
        let ids = ffi::ffi::get_encoding_ids(&batch_encoding_ptr, 0);
        let attention_mask = ffi::ffi::get_encoding_mask(&batch_encoding_ptr, 0);
        Encoding {
            input_ids: ids,
            attention_mask,
        }
    }

    pub fn decode(&self, ids: &[i32], skip_special_tokens: bool) -> String {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::decode(&self.inner, &ids.to_vec(), skip_special_tokens)
    }

    pub fn save(&self, path: &str) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::save(&self.inner, &path.to_string());
    }

    pub fn get_vocab_size(&self) -> usize {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        self.inner.get_vocab_size()
    }

    pub fn set_normalization_form(&mut self, form: &str) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::set_normalization_form(self.inner.pin_mut(), &form.to_string());
    }
}

impl Default for CharLevelTokenizer {
    fn default() -> Self {
        Self::new()
    }
}

// Legacy generic Tokenizer (kept for backward compatibility)
pub struct Tokenizer {
    inner: UniquePtr<ffi::ffi::TokenizerAdvanced>,
}

impl Tokenizer {
    pub fn new_bpe() -> Self {
        let config = default_config();
        let inner = ffi::ffi::create_bpe_tokenizer(&config, &"".to_string(), &"".to_string());
        Tokenizer { inner }
    }

    pub fn from_file(path: &str) -> Self {
        let inner = ffi::ffi::load_tokenizer(&path.to_string());
        Tokenizer { inner }
    }

    pub fn encode(&self, text: &str) -> Encoding {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        let batch_encoding_ptr = ffi::ffi::encode(&self.inner, &text.to_string());
        let ids = ffi::ffi::get_encoding_ids(&batch_encoding_ptr, 0);
        let attention_mask = ffi::ffi::get_encoding_mask(&batch_encoding_ptr, 0);
        Encoding {
            input_ids: ids,
            attention_mask,
        }
    }

    pub fn decode(&self, ids: &[i32], skip_special_tokens: bool) -> String {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::decode(&self.inner, &ids.to_vec(), skip_special_tokens)
    }

    pub fn save(&self, path: &str) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::save(&self.inner, &path.to_string());
    }

    pub fn train_from_files(&mut self, files: Vec<String>, vocab_size: usize) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::train_from_files(self.inner.pin_mut(), &files, vocab_size);
    }

    pub fn get_vocab_size(&self) -> usize {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        self.inner.get_vocab_size()
    }

    pub fn token_to_id(&self, token: &str) -> i32 {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::token_to_id(&self.inner, &token.to_string())
    }

    pub fn id_to_token(&self, id: i32) -> String {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::id_to_token(&self.inner, id)
    }

    pub fn add_special_tokens(&mut self, tokens: Vec<String>) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::add_special_tokens(self.inner.pin_mut(), &tokens);
    }

    pub fn set_normalization_form(&mut self, form: &str) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::set_normalization_form(self.inner.pin_mut(), &form.to_string());
    }

    pub fn set_strip_accents(&mut self, strip: bool) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::set_strip_accents(self.inner.pin_mut(), strip);
    }

    pub fn set_lowercase(&mut self, lowercase: bool) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::set_lowercase(self.inner.pin_mut(), lowercase);
    }

    pub fn add_pre_tokenizer_pattern(&mut self, pattern: &str) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::add_pre_tokenizer_pattern(self.inner.pin_mut(), &pattern.to_string());
    }

    pub fn create_bert_post_processor(&mut self, add_special_tokens: bool) {
        if self.inner.is_null() {
            panic!("Tokenizer not initialized");
        }
        ffi::ffi::create_bert_post_processor(self.inner.pin_mut(), add_special_tokens);
    }
}

// SAFETY: The underlying C++ tokenizers are designed to be used from a single thread at a time.
// These Send implementations allow moving tokenizers between threads, but not concurrent access.
// PyO3 will handle thread safety via Python's GIL.
unsafe impl Send for BPETokenizer {}
unsafe impl Send for WordPieceTokenizer {}
unsafe impl Send for UnigramTokenizer {}
unsafe impl Send for CharLevelTokenizer {}
unsafe impl Send for Tokenizer {}
