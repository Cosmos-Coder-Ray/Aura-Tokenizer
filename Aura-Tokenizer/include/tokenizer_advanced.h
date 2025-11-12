#pragma once

#include "tokenizer_config.h"
#include "tokenizer_types.h"
#include "icu_utils.h"
#include "unicode_normalizer.h"
#include "vocab.h"
#include "tokenizer_core.h"
#include "pre_tokenizer.h"
#include "post_processor.h"
#include "tokenizer_model.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <functional>
#include <nlohmann/json.hpp>

namespace auratokenizer {
    // Forward declarations
    class TokenizerModel;
    class PostProcessor;
    
    // Use the actual PreTokenizer class from the include
    using PreTokenizer = auratokenizer::PreTokenizer;

    // Forward declarations for batch encoding structures
    struct TokenizationOptions {
        bool add_special_tokens = true;
        bool return_attention_mask = false;
        bool return_token_type_ids = false;
        bool return_overflowing_tokens = false;
        bool return_length = false;
        int max_length = 512;
        bool pad_to_max_length = false;
        
        void load_from_json_string(const std::string& json_str) {
            try {
                auto json_obj = nlohmann::json::parse(json_str);
                if (json_obj.contains("add_special_tokens")) {
                    add_special_tokens = json_obj["add_special_tokens"];
                }
                if (json_obj.contains("return_attention_mask")) {
                    return_attention_mask = json_obj["return_attention_mask"];
                }
                if (json_obj.contains("return_token_type_ids")) {
                    return_token_type_ids = json_obj["return_token_type_ids"];
                }
                if (json_obj.contains("return_overflowing_tokens")) {
                    return_overflowing_tokens = json_obj["return_overflowing_tokens"];
                }
                if (json_obj.contains("return_length")) {
                    return_length = json_obj["return_length"];
                }
                if (json_obj.contains("max_length")) {
                    max_length = json_obj["max_length"];
                }
                if (json_obj.contains("pad_to_max_length")) {
                    pad_to_max_length = json_obj["pad_to_max_length"];
                }
            } catch (...) {
                // Use default values if parsing fails
            }
        }
    };

    struct BatchEncoding {
        std::vector<std::vector<int>> input_ids;
        std::vector<std::vector<int>> attention_mask;
        std::vector<std::vector<int>> token_type_ids;
        std::vector<std::vector<OffsetMapping>> offset_mapping;
        std::vector<size_t> length;
        std::vector<std::vector<std::vector<int>>> overflowing_tokens;
    };

    class TokenizerAdvanced {
    public:
        /**
         * Advanced Tokenizer supporting multiple modes and batch operations.
         * Thread-safe for batch encoding/decoding.
         */
        explicit TokenizerAdvanced(
            const TokenizerConfig& config,
            std::shared_ptr<TokenizerModel> model,
            std::shared_ptr<UnicodeNormalizer> normalizer,
            std::shared_ptr<PreTokenizer> pre_tokenizer,
            std::shared_ptr<PostProcessor> post_processor
        );

        /**
         * Encode a single string with options (normalization, special tokens, etc.).
         */
        BatchEncoding encode(const std::string& text, const TokenizationOptions& options = {}) const;
        /**
         * Encode a batch of strings with options.
         */
        BatchEncoding encode_batch(const std::vector<std::string>& texts, const TokenizationOptions& options = {}) const;
        /**
         * Decode a sequence of token IDs to string.
         */
        std::string decode(const std::vector<int>& ids, bool skip_special_tokens = true) const;
        /**
         * Decode a batch of token ID sequences to strings.
         */
        std::vector<std::string> decode_batch(const std::vector<std::vector<int>>& ids, bool skip_special_tokens = true) const;
        /**
         * Add special tokens to the vocabulary.
         */
        void add_special_tokens(const std::vector<std::string>& tokens);
        /**
         * Save the tokenizer state to disk.
         */
        void save(const std::string& path) const;
        /**
         * Load the tokenizer state from disk.
         */
        void load(const std::string& path);

    private:
        TokenizerConfig config_;
        std::shared_ptr<TokenizerModel> model_;
        std::shared_ptr<UnicodeNormalizer> normalizer_;
        std::shared_ptr<PreTokenizer> pre_tokenizer_;
        std::shared_ptr<PostProcessor> post_processor_;
        std::shared_ptr<TokenizerBase> tokenizer_impl_;
        std::shared_ptr<Vocab> vocab_;
        mutable std::mutex cache_mutex_;
        mutable std::unordered_map<std::string, std::vector<int>> tokenization_cache_;
        std::unordered_map<std::string, int> special_tokens_map_;
        std::unordered_map<int, std::string> id_to_special_token_;
    };
}