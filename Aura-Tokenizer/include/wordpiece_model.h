#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include "tokenizer_types.h"

namespace auratokenizer {

namespace models {

/**
 * WordPieceModel
 *
 * Implements the WordPiece tokenization algorithm.
 * This model is typically used with pre-tokenizers that split by whitespace.
 */
class WordPieceModel {
public:
    WordPieceModel() = default;

    /**
     * Initializes the WordPiece model with a vocabulary and an unknown token.
     * @param vocab A map from token string to its ID.
     * @param unk_token The token to use for unknown words.
     * @param max_input_chars_per_word The maximum number of characters a word can have.
     */
    void initialize(
        const std::unordered_map<std::string, int>& vocab,
        const std::string& unk_token,
        int max_input_chars_per_word = 100
    );

    /**
     * Tokenizes a single word into WordPieces.
     * @param word The input word.
     * @return A vector of WordPiece tokens.
     */
    std::vector<std::string> tokenize(const std::string& word) const;

    /**
     * Tokenizes a batch of words into WordPieces.
     * @param words A vector of input words.
     * @return A vector of vectors of WordPiece tokens.
     */
    std::vector<std::vector<std::string>> batch_tokenize(const std::vector<std::string>& words) const;

    /**
     * Returns the vocabulary of the WordPiece model.
     */
    const std::unordered_map<std::string, int>& get_vocab() const { return vocab_; }

    /**
     * Returns the unknown token.
     */
    const std::string& get_unk_token() const { return unk_token_; }

private:
    std::unordered_map<std::string, int> vocab_;
    std::string unk_token_;
    int max_input_chars_per_word_;
};

} // namespace models

} // namespace auratokenizer
