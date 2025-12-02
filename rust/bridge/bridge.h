#pragma once
#include "tokenizer_advanced.h"
#include "tokenizer_config.h"
#include <memory>
#include <string>
#include <vector>
#include "rust/cxx.h"

namespace auratokenizer {

struct RustTokenizerConfig;

// Factory functions for each tokenizer type
std::unique_ptr<TokenizerAdvanced> create_bpe_tokenizer(
    const RustTokenizerConfig& config,
    const rust::String& vocab_path,
    const rust::String& merges_path
);
std::unique_ptr<TokenizerAdvanced> create_wordpiece_tokenizer(
    const RustTokenizerConfig& config,
    const rust::String& vocab_path
);
std::unique_ptr<TokenizerAdvanced> create_unigram_tokenizer(
    const RustTokenizerConfig& config,
    const rust::String& vocab_path
);
std::unique_ptr<TokenizerAdvanced> create_charlevel_tokenizer(
    const RustTokenizerConfig& config
);
std::unique_ptr<TokenizerAdvanced> load_tokenizer(const rust::String& path);

// Helpers
rust::Vec<int> get_encoding_ids(const BatchEncoding& encoding, size_t index);
rust::Vec<int> get_encoding_mask(const BatchEncoding& encoding, size_t index);
void add_special_tokens(TokenizerAdvanced& tokenizer, const rust::Vec<rust::String>& tokens);
void save(const TokenizerAdvanced& tokenizer, const rust::String& path);
std::unique_ptr<BatchEncoding> encode(const TokenizerAdvanced& tokenizer, const rust::String& text);
rust::String decode(const TokenizerAdvanced& tokenizer, const rust::Vec<int>& ids, bool skip_special_tokens);
int token_to_id(const TokenizerAdvanced& tokenizer, const rust::String& token);
rust::String id_to_token(const TokenizerAdvanced& tokenizer, int id);

// Training helper
void train_from_files(TokenizerAdvanced& tokenizer, const rust::Vec<rust::String>& files, size_t vocab_size);

// Configuration helpers
void set_normalization_form(TokenizerAdvanced& tokenizer, const rust::String& form);
void set_strip_accents(TokenizerAdvanced& tokenizer, bool strip);
void set_lowercase(TokenizerAdvanced& tokenizer, bool lowercase);
void add_pre_tokenizer_pattern(TokenizerAdvanced& tokenizer, const rust::String& pattern);
void create_bert_post_processor(TokenizerAdvanced& tokenizer, bool add_special_tokens);

} // namespace auratokenizer
