#include "tokenizer_encoder.h"
#include "unicode_normalizer.h"

#include <fstream>
#include <memory>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace auratokenizer {

    ////////////////////////////////////////////////////////////////////////////////
    // Constructor
    ////////////////////////////////////////////////////////////////////////////////

    Encoder::Encoder(const TokenizerConfig& config)
        : TokenizerBase(config),
          config_(config)
    {
        vocab_ = std::make_unique<Vocab>();
        initialize_special_tokens();
    }

    ////////////////////////////////////////////////////////////////////////////////
    // normalize_and_pretokenize
    ////////////////////////////////////////////////////////////////////////////////

    std::string Encoder::normalize_and_pretokenize(const std::string& txt) const {
        std::string normalized = txt;

        // Apply Unicode normalization if configured
        if (config_.normalization != NormalizationForm::NONE) {
            UnicodeNormalizer normalizer(config_);
            normalized = normalizer.normalize(txt);
        }

        // Apply lowercase if configured
        if (config_.lowercase) {
            std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                [](unsigned char c) { return std::tolower(c); });
        }

        // In this implementation, "pre_tokenize" simply splits on whitespace
        std::vector<std::string> words;
        std::istringstream iss(normalized);
        std::string word;
        while (iss >> word) {
            if (!word.empty()) {
                words.push_back(word);
            }
        }
        
        // Join words back into a string
        std::ostringstream oss;
        for (size_t i = 0; i < words.size(); ++i) {
            if (i > 0) oss << " ";
            oss << words[i];
        }
        return oss.str();
    }

    /* ------------------------------ Helper: maybe_push ------------------------------ */

    void Encoder::maybe_push(std::vector<int>& ids, SpecialTokenType t) const {
        int id = vocab_->get_special_token_id(t);
        if (id >= 0) {
            ids.push_back(id);
        }
    }

    /* ------------------------- Core Encode / Encode_to_ids ------------------------- */

    // --- TokenizerBase Interface ---
    std::vector<Token> Encoder::encode(const std::string& text) {
        std::vector<int> ids = encode_to_ids(text);
        std::vector<Token> tokens;
        tokens.reserve(ids.size());
        size_t offset = 0;
        for (int id : ids) {
            std::string tok = vocab_->get_token(id);
            tokens.emplace_back(id, tok, vocab_->is_special_token_id(id), OffsetMapping{static_cast<int>(offset), static_cast<int>(offset + tok.size())});
            offset += tok.size();
        }
        return tokens;
    }

    std::vector<int> Encoder::encode_to_ids(const std::string& text) {
        // 1) Basic normalization & "pre‐tokenize" (whitespace split)
        std::vector<std::string> words = pre_tokenize(text);
        std::vector<int> ids;
        ids.reserve(words.size() + 2); // +2 for optional BOS/EOS

        // 2) Possibly add BOS
        if (config_.add_special_tokens && !config_.bos_token.empty()) {
            ids.push_back(vocab_->get_token_id(config_.bos_token));
        }

        // 3) For each "word," break into subwords (in this simple version, char-level)
        for (auto const& word : words) {
            std::vector<std::string> subwords = word_tokenize(word);
            for (auto const& sub : subwords) {
                int id = vocab_->get_token_id(sub);
                if (id == -1) {
                    // Unknown → UNK
                    id = vocab_->get_token_id(config_.unk_token);
                }
                ids.push_back(id);
            }
        }

        // 4) Possibly add EOS
        if (config_.add_special_tokens && !config_.eos_token.empty()) {
            ids.push_back(vocab_->get_token_id(config_.eos_token));
        }

        return ids;
    }

    /* ------------------------- Core Decode / Decode_from_ids ------------------------- */

    std::string Encoder::decode(const std::vector<Token>& tokens) {
        std::vector<int> ids;
        ids.reserve(tokens.size());
        for (const auto& t : tokens) {
            ids.push_back(t.id);
        }
        return decode_from_ids(ids);
    }

    std::string Encoder::decode_from_ids(const std::vector<int>& ids) {
        std::string text;
        bool skip_special = !config_.add_special_tokens;

        for (size_t i = 0; i < ids.size(); ++i) {
            int id = ids[i];
            if (skip_special && vocab_->is_special_token_id(id)) {
                continue;
            }

            std::string token = vocab_->get_token(id);
            if (token.empty()) {
                token = config_.unk_token;
            }

            // Handle special tokens
            if (token == config_.sep_token) {
                text += " ";
            }
            else if (token == config_.cls_token ||
                token == config_.bos_token ||
                token == config_.eos_token) {
                // Skip
                continue;
            }
            else {
                text += token;
            }
        }

        return post_process(text);
    }

    /* ------------------------------ Persistence: save/load ------------------------------ */

    void Encoder::save(const std::string& path) {
        if (path.empty()) {
            throw TokenizerException("Empty path provided for saving");
        }

        std::ofstream file(path, std::ios::binary);
        if (!file) {
            throw TokenizerException("Failed to open file for saving: " + path);
        }

        // Save configuration
        std::string config_str = config_.to_string();
        uint32_t config_len = static_cast<uint32_t>(config_str.size());
        file.write(reinterpret_cast<const char*>(&config_len), sizeof(config_len));
        file.write(config_str.c_str(), config_len);

        // Save vocabulary
        vocab_->save(file);

        file.close();
    }

    void Encoder::load(const std::string& path) {
        if (path.empty()) {
            throw TokenizerException("Empty path provided for loading");
        }

        std::ifstream file(path, std::ios::binary);
        if (!file) {
            throw TokenizerException("Failed to open file for loading: " + path);
        }

        // Load configuration
        uint32_t config_len;
        file.read(reinterpret_cast<char*>(&config_len), sizeof(config_len));
        std::string config_str(config_len, '\0');
        file.read(&config_str[0], config_len);
        config_ = TokenizerConfig::from_string(config_str);

        // Load vocabulary
        vocab_->load(file);

        file.close();
        initialize_special_tokens();
    }

    /* ------------------------------ Special Token Helpers ------------------------------ */

    int Encoder::get_special_token_id(SpecialTokenType type) const {
        return vocab_->get_special_token_id(type);
    }

    std::string Encoder::get_special_token_text(SpecialTokenType type) const {
        return vocab_->get_special_token_text(type);
    }

    /* ------------------------------ Batch Processing ------------------------------ */

    std::vector<std::vector<int>> Encoder::batch_encode(const std::vector<std::string>& texts) {
        std::vector<std::vector<int>> results;
        results.reserve(texts.size());

        for (const auto& text : texts) {
            results.push_back(encode_to_ids(text));
        }

        return results;
    }

    std::vector<std::string> Encoder::batch_decode(const std::vector<std::vector<int>>& ids_batch) {
        std::vector<std::string> results;
        results.reserve(ids_batch.size());

        for (const auto& ids : ids_batch) {
            results.push_back(decode_from_ids(ids));
        }

        return results;
    }

    /* ------------------------------ Offset Mappings ------------------------------ */

    std::vector<OffsetMapping> Encoder::get_offset_mappings(const std::string& text) const {
        std::vector<Token> tokens = const_cast<Encoder*>(this)->encode(text);
        std::vector<OffsetMapping> mappings;
        mappings.reserve(tokens.size());

        for (const auto& token : tokens) {
            mappings.push_back(token.offset);
        }

        return mappings;
    }

    std::vector<OffsetMapping> Encoder::get_offset_mappings(const std::vector<Token>& tokens) const {
        std::vector<OffsetMapping> mappings;
        mappings.reserve(tokens.size());

        for (const auto& token : tokens) {
            mappings.push_back(token.offset);
        }

        return mappings;
    }

    OffsetMapping Encoder::get_token_offset(const Token& token) const {
        return token.offset;
    }

    /* ------------------------------ Training ------------------------------ */

    void Encoder::train(const std::vector<std::string>& texts, size_t vocab_size) {
        // Simple training implementation
        // In a real implementation, you'd use a proper trainer
        throw TokenizerException("Training not implemented in basic Encoder");
    }

    void Encoder::train(const std::string& text, size_t vocab_size) {
        // Simple training implementation
        throw TokenizerException("Training not implemented in basic Encoder");
    }

    /* ------------------------------ Special Token Management ------------------------------ */

    void Encoder::add_special_tokens(const std::vector<std::string>& tokens) {
        for (const auto& token : tokens) {
            vocab_->add_special_token(token, SpecialTokenType::UNK);
        }
    }

    std::vector<std::string> Encoder::get_special_tokens() const {
        return vocab_->get_special_tokens();
    }

    bool Encoder::is_special_token(const std::string& token) const {
        return vocab_->is_special_token(token);
    }

    const TokenizerConfig& Encoder::get_config() const {
        return config_;
    }

    void Encoder::set_config(const TokenizerConfig& config) {
        config_ = config;
    }

    void Encoder::set_vocab(std::shared_ptr<Vocab> vocab) {
        // Convert shared_ptr to unique_ptr for internal storage
        // Note: This is a simplified implementation
        vocab_ = std::make_unique<Vocab>(*vocab);
        initialize_special_tokens();
    }

    /* ------------------------------ Private Helper Methods ------------------------------ */

    void Encoder::initialize_special_tokens() {
        // Add default special tokens
        if (!config_.unk_token.empty()) {
            vocab_->add_special_token(config_.unk_token, SpecialTokenType::UNK);
        }
        if (!config_.pad_token.empty()) {
            vocab_->add_special_token(config_.pad_token, SpecialTokenType::PAD);
        }
        if (!config_.bos_token.empty()) {
            vocab_->add_special_token(config_.bos_token, SpecialTokenType::BOS);
        }
        if (!config_.eos_token.empty()) {
            vocab_->add_special_token(config_.eos_token, SpecialTokenType::EOS);
        }
        if (!config_.mask_token.empty()) {
            vocab_->add_special_token(config_.mask_token, SpecialTokenType::MASK);
        }
        if (!config_.sep_token.empty()) {
            vocab_->add_special_token(config_.sep_token, SpecialTokenType::SEP);
        }
        if (!config_.cls_token.empty()) {
            vocab_->add_special_token(config_.cls_token, SpecialTokenType::CLS);
        }
    }

    std::vector<std::string> Encoder::pre_tokenize(const std::string& text) {
        std::string normalized = normalize_and_pretokenize(text);
        std::vector<std::string> words;
        std::istringstream iss(normalized);
        std::string word;
        while (iss >> word) {
            if (!word.empty()) {
                words.push_back(word);
            }
        }
        return words;
    }

    std::vector<std::string> Encoder::word_tokenize(const std::string& text) {
        std::vector<std::string> tokens;
        for (char c : text) {
            tokens.emplace_back(1, c);
        }
        return tokens;
    }

    std::string Encoder::post_process(const std::string& text) const {
        std::string result = text;

        // Remove extra whitespace
        std::string::iterator new_end = std::unique(result.begin(), result.end(),
            [](char a, char b) { return std::isspace(a) && std::isspace(b); });
        result.erase(new_end, result.end());

        // Remove control characters
        result.erase(std::remove_if(result.begin(), result.end(),
            [](unsigned char c) { return std::iscntrl(c); }), result.end());

        // Apply lowercase if configured
        if (config_.lowercase) {
            std::transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c) { return std::tolower(c); });
        }

        // Normalize whitespace
        std::istringstream iss(result);
        std::ostringstream oss;
        std::string word;
        bool first = true;
        while (iss >> word) {
            if (!first) oss << " ";
            oss << word;
            first = false;
        }
        return oss.str();
    }

    bool Encoder::is_chinese_char(char32_t c) {
        return (c >= 0x4E00 && c <= 0x9FFF) ||   // CJK Unified Ideographs
               (c >= 0x3400 && c <= 0x4DBF) ||   // CJK Unified Ideographs Extension A
               (c >= 0x20000 && c <= 0x2A6DF) || // CJK Unified Ideographs Extension B
               (c >= 0x2A700 && c <= 0x2B73F) || // CJK Unified Ideographs Extension C
               (c >= 0x2B740 && c <= 0x2B81F) || // CJK Unified Ideographs Extension D
               (c >= 0x2B820 && c <= 0x2CEAF) || // CJK Unified Ideographs Extension E
               (c >= 0xF900 && c <= 0xFAFF) ||   // CJK Compatibility Ideographs
               (c >= 0x2F800 && c <= 0x2FA1F);   // CJK Compatibility Ideographs Supplement
    }

    int Encoder::utf8_sequence_length(unsigned char lead_byte) {
        if (lead_byte < 0x80) return 1;
        if ((lead_byte & 0xE0) == 0xC0) return 2;
        if ((lead_byte & 0xF0) == 0xE0) return 3;
        if ((lead_byte & 0xF8) == 0xF0) return 4;
        return 1; // Invalid UTF-8, treat as single byte
    }

    bool Encoder::utf8_is_continuation_byte(unsigned char byte) {
        return (byte & 0xC0) == 0x80;
    }

} // namespace auratokenizer
