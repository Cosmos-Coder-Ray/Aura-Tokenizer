#pragma once

#include "tokenizer_core.h"
#include "vocab.h"
#include "unicode_normalizer.h"
#include "bpe_trainer.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

namespace auratokenizer {

    /**
     * @class BPETokenizer
     * @brief Implements Byte-Pair Encoding (BPE) tokenization.
     *
     * Supports encoding, decoding, training, saving/loading, and special token management.
     * Provides access to BPE merge rules and configuration.
     */
    class BPETokenizer : public TokenizerBase {
    public:
        /**
         * @brief Encode a batch of texts into token IDs.
         * @param texts Vector of input UTF-8 strings.
         * @return Vector of vectors of token IDs.
         */
        std::vector<std::vector<int>> batch_encode(const std::vector<std::string>& texts);
        
        /**
         * @brief Decode a batch of token IDs into texts.
         * @param ids_batch Vector of vectors of token IDs.
         * @return Vector of decoded strings.
         */
        std::vector<std::string> batch_decode(const std::vector<std::vector<int>>& ids_batch);

        /**
         * @brief Construct a BPETokenizer with optional configuration.
         * @param config Tokenizer configuration.
         */
        explicit BPETokenizer(const TokenizerConfig& config = TokenizerConfig());

        /** 
         * @brief Virtual destructor.
         */
        virtual ~BPETokenizer();

        /**
         * @brief Set the vocabulary for the tokenizer.
         * @param vocab Shared pointer to a Vocab object.
         */
        void set_vocab(std::shared_ptr<Vocab> vocab);

        /**
         * @brief Encode text into a sequence of Token structs.
         * @param text Input UTF-8 string.
         * @return Vector of Token structs.
         */
        std::vector<Token> encode(const std::string& text);
        /**
         * @brief Encode text into a sequence of token IDs.
         * @param text Input UTF-8 string.
         * @return Vector of token IDs.
         */
        std::vector<int> encode_to_ids(const std::string& text) override;
        /**
         * @brief Decode a sequence of Token structs back to text.
         * @param tokens Vector of Token structs.
         * @return Decoded UTF-8 string.
         */
        std::string decode(const std::vector<Token>& tokens);
        
        /**
         * @brief Decode a sequence of token IDs back to text.
         * @param ids Vector of token IDs.
         * @return Decoded UTF-8 string.
         */
        std::string decode_from_ids(const std::vector<int>& ids);

        /**
         * @brief Save the tokenizer state to a file or folder.
         * @param path Output path.
         */
        void save(const std::string& path);
        
        /**
         * @brief Load the tokenizer state from a file or folder.
         * @param path Input path.
         */
        void load(const std::string& path);
        /**
         * @brief Train the tokenizer on a corpus.
         * @param corpus Input corpus (vector of strings).
         * @param vocab_size Desired vocabulary size.
         */
        void train(const std::vector<std::string>& corpus, size_t vocab_size);

        /**
         * @brief Add special tokens to the tokenizer.
         * @param tokens Vector of special token strings.
         */
        void add_special_tokens(const std::vector<std::string>& tokens);

        /**
         * @brief Get all special tokens.
         * @return Vector of special token strings.
         */
        std::vector<std::string> get_special_tokens() const;

        /**
         * @brief Check if a string is a special token.
         * @param token Token string.
         * @return True if special token, false otherwise.
         */
        bool is_special_token(const std::string& token) const;

        /**
         * @brief Get the ID of a special token by type.
         * @param type Special token type.
         * @return Token ID.
         */
        int get_special_token_id(SpecialTokenType type) const;

        /**
         * @brief Get the current tokenizer configuration.
         * @return TokenizerConfig reference.
         */
        const TokenizerConfig& get_config() const;

        /**
         * @brief Set the tokenizer configuration.
         * @param config New configuration.
         */
        void set_config(const TokenizerConfig& config);

        /**
         * @brief Get the BPE merge rules.
         * @return Vector of merge rule pairs.
         */
        const std::vector<std::pair<std::string, std::string>>& get_merge_rules() const;
        /**
         * @brief Set the BPE merge rules.
         * @param merges Vector of merge rule strings.
         */
        void set_merge_rules(const std::vector<std::string>& merges);

    private:
        UnicodeNormalizer normalizer_;
        std::shared_ptr<Vocab> vocab_;
        TokenizerConfig config_;
        std::unordered_map<SpecialTokenType, std::string> special_tokens_;
        std::vector<std::pair<std::string, std::string>> merge_rules_;
        std::unordered_map<std::string, int> merge_ranks_;

        void initialize_special_tokens();
        std::vector<std::string> pre_tokenize(const std::string& text) const;
        std::vector<Token> encode_bpe(const std::string& word) const;
        void build_merge_ranks();
        std::string post_process_text(const std::string& text) const;
    };
}