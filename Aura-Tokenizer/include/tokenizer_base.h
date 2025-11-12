#pragma once

#include "tokenizer_types.h"
#include "token.h"
#include "utf8_utils.h"
#include "icu_utils.h"
#include <string>
#include <vector>

namespace auratokenizer {

    /**
     * TokenizerBase
     *
     * Abstract base class for tokenizers. Provides:
     *  - Pure virtual methods for encode/decode, training, I/O, and special token handling.
     *  - Unicode-aware protected helpers: split_text, normalize_text, and is_whitespace.
     *
     * Derived classes (e.g., BPE_Tokenizer) must override all pure virtual methods.
     */
    class TokenizerBase {
    public:
        virtual ~TokenizerBase() = default;

        // ─── Core tokenization methods ───

        /**
         * Convert input UTF-8 text into a sequence of Token structs.
         * Derived classes must implement.
         */
        virtual std::vector<Token> encode(const std::string& text) = 0;

        /**
         * Convert input UTF-8 text into a sequence of token IDs.
         * Derived classes must implement.
         */
        virtual std::vector<int> encode_to_ids(const std::string& text) = 0;

        /**
         * Convert a sequence of Token structs back to UTF-8 text.
         * Derived classes must implement.
         */
        virtual std::string decode(const std::vector<Token>& tokens) = 0;

        /**
         * Convert a sequence of token IDs back to UTF-8 text.
         * Derived classes must implement.
         */
        virtual std::string decode_from_ids(const std::vector<int>& ids) = 0;

        // ─── Training / Serialization methods ───

        /**
         * Train a tokenizer model on a corpus of strings with desired vocabulary size.
         * Derived classes must implement.
         */
        virtual void train(const std::vector<std::string>& texts, size_t vocab_size) = 0;

        /**
         * Save tokenizer state (vocab, merges, config, etc.) to a file or folder path.
         * Derived classes must implement.
         */
        virtual void save(const std::string& path) = 0;

        /**
         * Load tokenizer state from a file or folder path.
         * Derived classes must implement.
         */
        virtual void load(const std::string& path) = 0;

        // ─── Special token handling ───

        /**
         * Add a list of special tokens (e.g., "[PAD]", "[CLS]", etc.) into the tokenizer model.
         * Derived classes must implement.
         */
        virtual void add_special_tokens(const std::vector<std::string>& tokens) = 0;

        /**
         * Get a vector of all special token strings.
         * Derived classes must implement.
         */
        virtual std::vector<std::string> get_special_tokens() const = 0;

        /**
         * Check if a given string is recognized as a special token.
         * Derived classes must implement.
         */
        virtual bool is_special_token(const std::string& token) const = 0;

        // ─── Configuration access ───

        /**
         * Retrieve current tokenizer configuration (e.g., normalization, padding, etc.).
         * Derived classes must implement.
         */
        virtual TokenizerConfig get_config() const = 0;

        /**
         * Update tokenizer configuration.
         * Derived classes must implement.
         */
        virtual void set_config(const TokenizerConfig& config) = 0;

    protected:
        /**
         * Split a UTF-8 string into “words” using whitespace (default implementation).
         * Example: "Hello world!" → ["Hello", "world!"]
         */
        std::vector<std::string> split_text(const std::string& text);

        /**
         * Normalize a UTF-8 string (default: returns input unchanged).
         * Derived classes should override for custom normalization.
         */
        virtual std::string normalize_text(const std::string& text);

        /**
         * Returns true if the given character is Unicode whitespace (ICU-aware).
         */
        static bool is_whitespace(char32_t c);
    };

} // namespace auratokenizer