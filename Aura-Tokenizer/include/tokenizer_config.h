#pragma once

#include <string>
#include <ostream>
#include <istream>
#include <vector>
#include <unordered_map>
#include "tokenizer_types.h"

namespace auratokenizer {
    class TokenizerConfig {
    public:
        // Vocabulary
        size_t vocab_size = 30000;
        std::string model_type = "bpe";
        ModelType base_model = ModelType::CUSTOM;
        bool add_special_tokens = true;
        bool add_byte_fallback = true;

        // Special tokens
        std::string unk_token = "[UNK]";
        std::string pad_token = "[PAD]";
        std::string bos_token = "[BOS]";
        std::string eos_token = "[EOS]";
        std::string mask_token = "[MASK]";
        std::string sep_token = "[SEP]";
        std::string cls_token = "[CLS]";

        // Added tokens (custom tokens)
        std::unordered_map<std::string, std::string> added_tokens;

        // Chat template for conversation models
        std::string chat_template;

        // Pre-tokenizer patterns
        std::vector<std::string> pre_tokenizer_patterns;

        // Normalization
        bool lowercase = false;
        bool strip_accents = false;
        NormalizationForm normalization = NormalizationForm::NFC;
        bool normalize_whitespace = true;
        bool remove_control_chars = true;
        bool remove_diacritics = false;

        // Training
        size_t min_frequency = 2;
        size_t max_tokens = 1000000;
        bool use_regex = false;
        std::string regex_pattern;

        // Padding / Truncation
        int max_length = 512;
        bool pad_to_max_length = false;
        TruncationStrategy truncation_strategy = TruncationStrategy::LONGEST_FIRST;

        // Serialization
        void save(std::ostream& out) const;
        void load(std::istream& in);
        void load_from_json_string(const std::string& json_str);
        void add_special_token(const std::string& token, int id);
        
        // String conversion methods
        std::string to_string() const;
        static TokenizerConfig from_string(const std::string& str);

        TokenizerConfig() = default;
        TokenizerConfig(const TokenizerConfig& other) = default;

    private:
        void write_string(std::ostream& out, const std::string& str) const;
        static std::string read_string(std::istream& in);
    };
}