#pragma once

#include "tokenizer_core.h"      // For TokenizerBase, OffsetMapping, TokenizerException
#include "vocab.h"               // For Vocab, Token, SpecialTokenType
#include "tokenizer_config.h"    // For TokenizerConfig
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace auratokenizer {

    /**
     * Encoder
     *
     * A high-performance inference‐time tokenizer/encoder.  Internally:
     *   • Holds a Vocab (as unique_ptr<Vocab>) for token‐ID lookups
     *   • Applies Unicode normalization, simple whitespace‐based pre‐tokenization,
     *     and optional byte‐fallback / special‐token insertion
     *   • Supports encode(), decode(), batch_encode(), offset mappings, saving/loading, etc.
     */
    class Encoder : public TokenizerBase {
    public:
        // Constructor: builds an empty Vocab and registers special tokens
        explicit Encoder(const TokenizerConfig& config);

        ~Encoder() override = default;

        // Delete copy-construct and copy-assign (Vocab is unique_ptr)
        Encoder(const Encoder&) = delete;
        Encoder& operator=(const Encoder&) = delete;

        // Allow move-construct and move-assign
        Encoder(Encoder&&) noexcept = default;
        Encoder& operator=(Encoder&&) noexcept = default;

        /** TokenizerBase Interface */
        std::vector<Token> encode(const std::string& text) override;
        std::vector<int> encode_to_ids(const std::string& text) override;
        std::string decode(const std::vector<Token>& tokens) override;
        std::string decode_from_ids(const std::vector<int>& ids) override;
        std::vector<std::vector<int>> batch_encode(const std::vector<std::string>& texts) override;
        std::vector<std::string> batch_decode(const std::vector<std::vector<int>>& ids) override;
        void train(const std::vector<std::string>& texts, size_t vocab_size) override;
        void save(const std::string& path) override;
        void load(const std::string& path) override;
        void add_special_tokens(const std::vector<std::string>& tokens) override;
        std::vector<std::string> get_special_tokens() const override;
        bool is_special_token(const std::string& token) const override;
        const TokenizerConfig& get_config() const override;
        void set_config(const TokenizerConfig& config) override;
        void set_vocab(std::shared_ptr<Vocab> vocab) override;

        /** ─────── Offset‐Mapping Methods ─────── */

        // Compute (start,end) offsets for each token in the raw input
        std::vector<OffsetMapping> get_offset_mappings(const std::string& text) const;

        // Overload: if you already have the vector<Token>, extract offsets
        std::vector<OffsetMapping> get_offset_mappings(const std::vector<Token>& tokens) const;

        // Get a single token's offset (in the original input) if stored
        OffsetMapping get_token_offset(const Token& token) const;

        /** ─────── Batch Encoding Methods ─────── */

        struct BatchEncoding {
            std::vector<std::vector<int>>            input_ids;
            std::vector<std::vector<int>>            attention_mask;
            std::vector<std::vector<int>>            token_type_ids;
            std::vector<std::vector<OffsetMapping>>  offset_mapping;
            std::vector<int>                         length;
            std::vector<std::vector<std::vector<int>>> overflowing_tokens;
        };

        // Encode a batch of single‐sequences (with optional special tokens, padding, etc.)
        BatchEncoding encode_batch(
            const std::vector<std::string>& texts,
            bool add_special_tokens = true,
            bool return_attention_mask = true,
            bool return_token_type_ids = true,
            bool return_overflowing_tokens = false,
            bool return_length = false
        ) const;

        // Encode a batch of text‐pairs (sequence A + sequence B) with [SEP] in between
        BatchEncoding encode_batch_pairs(
            const std::vector<std::pair<std::string, std::string>>& text_pairs,
            bool add_special_tokens = true,
            bool return_attention_mask = true,
            bool return_token_type_ids = true,
            bool return_overflowing_tokens = false,
            bool return_length = false
        ) const;

        /** ─────── Training & Special‐Token Management ─────── */

        // Train a Unigram model (or any trainer) over a raw corpus (string)
        void train(const std::string& text, size_t vocab_size);

        /** ─────── Special Token Helpers ─────── */
        
        // Get the ID of a special token type
        int get_special_token_id(SpecialTokenType type) const;
        
        // Get the text of a special token type
        std::string get_special_token_text(SpecialTokenType type) const;

    private:
        // Owned vocabulary (unique_ptr ensures proper ownership)
        std::unique_ptr<Vocab>                    vocab_;

        // Current tokenizer settings
        TokenizerConfig                            config_;

        // Map from token text to its SpecialTokenType (e.g. "[PAD]" → PAD)
        std::unordered_map<std::string, SpecialTokenType> special_tokens_;

        // ─────── Private Helper Methods ───────

        // Insert exactly one copy of each configured special token into vocab_
        void initialize_special_tokens();

        // Lowercase + (future) other normalization steps before pre_tokenize
        std::string normalize_and_pretokenize(const std::string& txt) const;

        // If a token type is special, push its ID into the vector
        void maybe_push(std::vector<int>& ids, SpecialTokenType t) const;

        // Compute a single token's offset by searching in the original text
        OffsetMapping calculate_token_offset(
            const std::string& original_text,
            const std::string& token_text,
            size_t start_pos
        ) const;

        // Given the entire vector<Token>, produce a same‐length vector<OffsetMapping>
        std::vector<OffsetMapping> calculate_offset_mappings(
            const std::string& original_text,
            const std::vector<Token>& tokens
        ) const;

        // Truncate a sequence of IDs to max_length using the chosen strategy
        std::vector<int> truncate_sequence(
            const std::vector<int>& ids,
            int max_length,
            TruncationStrategy strategy
        ) const;

        // Pad a sequence of IDs to max_length using the chosen padding direction
        std::vector<int> pad_sequence(
            const std::vector<int>& ids,
            int max_length,
            PaddingDirection direction
        ) const;

        // Create an attention mask (1 = real token, 0 = padding) for a padded sequence
        std::vector<int> create_attention_mask(
            const std::vector<int>& ids,
            int max_length
        ) const;

        // Create token‐type IDs (0 for first sequence, 1 for second) if is_pair==true
        std::vector<int> create_token_type_ids(
            const std::vector<int>& ids,
            int max_length,
            bool is_pair = false
        ) const;

        // Compute the padded length (possibly rounding up to pad_to_multiple_of_size)
        int get_padding_length(int sequence_length) const;

        // A very simple whitespace‐based "pre_tokenization"
        std::vector<std::string> pre_tokenize(const std::string& text);

        // A naive character‐level word_tokenizer (splits every character)
        std::vector<std::string> word_tokenize(const std::string& text);

        // Post‐processing, e.g. remove extra spaces, handle Chinese characters, etc.
        std::string post_process(const std::string& text) const;

        // Detect whether a 32‐bit codepoint is a "Chinese" CJK character
        static bool is_chinese_char(char32_t c);

        // Inline UTF-8 helpers (optional—also defined in tokenizer_core.cpp)
        static int utf8_sequence_length(unsigned char lead_byte);
        static bool utf8_is_continuation_byte(unsigned char byte);
    };

} // namespace auratokenizer
