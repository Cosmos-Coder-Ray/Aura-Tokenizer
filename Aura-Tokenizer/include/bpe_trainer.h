#pragma once

#include "tokenizer_config.h"
#include "unicode_normalizer.h"
#include "vocab.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <utility>
#include <memory>

namespace auratokenizer {

    class BPETrainer {
    public:
        BPETrainer(const TokenizerConfig& config = TokenizerConfig());
        ~BPETrainer() = default;

        void set_vocab_size(size_t size);
        void set_min_frequency(size_t freq);
        void set_normalizer(const UnicodeNormalizer& norm);

        void train(const std::vector<std::string>& corpus, std::shared_ptr<Vocab> vocab, size_t vocab_size);
        void train_from_file(const std::string& file_path, std::shared_ptr<Vocab> vocab);
        void train_from_files(const std::vector<std::string>& file_paths, std::shared_ptr<Vocab> vocab);

        const std::vector<std::pair<std::string, std::string>>& get_merge_rules() const {
            return merge_rules_;
        }
        const std::unordered_map<std::string, int>& get_vocab_counts() const {
            return word_counts_;
        }

    private:
        TokenizerConfig config_;
        size_t min_frequency_;
        size_t vocab_size_;
        UnicodeNormalizer normalizer_;

        std::unordered_map<std::string, int> word_counts_;

        struct Pair {
            std::string first;
            std::string second;
            bool operator==(const Pair& other) const { return first == other.first && second == other.second; }
        };

        struct PairHash {
            size_t operator()(const Pair& p) const { return std::hash<std::string>()(p.first) ^ (std::hash<std::string>()(p.second) << 1); }
        };

        std::unordered_map<Pair, int, PairHash> pair_counts_;
        std::vector<std::pair<std::string, std::string>> merge_rules_;

        std::unordered_map<std::string, int> get_word_counts(const std::vector<std::string>& corpus);
        void initialize_vocab(std::shared_ptr<Vocab> vocab, const std::unordered_map<std::string, int>& initial_vocab_counts);
        void learn_bpe_merges(std::shared_ptr<Vocab> vocab);

        std::unordered_map<Pair, int, PairHash> get_pair_counts(const std::unordered_map<std::string, int>& word_counts) const;
        void merge_pair_counts(const std::string& first, const std::string& second, std::unordered_map<std::string, int>& word_counts, const Pair& best_pair, std::string& new_token);
    };

}