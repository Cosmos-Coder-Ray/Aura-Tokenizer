#pragma once

#include "vocab.h"
#include "tokenizer_config.h"
#include "tokenizer_core.h"
#include "tokenizer_exception.h"
#include "token.h"
#include "bpe_tokenizer.h"
#include "unigram_trainer.h"
#include "wordpiece_model.h"

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <random>

namespace auratokenizer {

    // Forward declaration
    class Vocab;

    // PairHash struct definition (moved before use)
    struct PairHash {
        template<typename T1, typename T2>
        std::size_t operator()(const std::pair<T1, T2>& p) const {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);
            return h1 ^ (h2 << 1);
        }
    };

    /**
     * @brief Concrete class for tokenizer models
     *
     * This class holds the data for different tokenizer models (BPE, Unigram, WordPiece)
     * and provides methods to access their specific data (vocab, merges, scores).
     */
    class TokenizerModel {
    public:
        TokenizerModel();
        ~TokenizerModel() = default;

        // Vocabulary management
        void add_token_to_vocab(const std::string& token, int id);
        void add_token_with_score(const std::string& token, float score);
        void add_merge(const std::string& merge_str);

        const std::unordered_map<std::string, int>& get_vocab() const { return vocab_; }
        const std::unordered_map<std::string, float>& get_scores() const { return scores_; }
        const std::vector<std::string>& get_merges() const { return merges_; }

        void set_algorithm(TokenizationAlgorithm algo) { algorithm_ = algo; }
        TokenizationAlgorithm get_algorithm() const { return algorithm_; }

    private:
        std::unordered_map<std::string, int> vocab_;
        std::unordered_map<std::string, float> scores_; // For Unigram
        std::vector<std::string> merges_; // For BPE
        TokenizationAlgorithm algorithm_;
    };

} // namespace auratokenizer