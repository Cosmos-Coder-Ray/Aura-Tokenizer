#include "bpe_trainer.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace auratokenizer {

    BPETrainer::BPETrainer(const TokenizerConfig& config)
        : config_(config),
        min_frequency_(config.min_frequency),
        vocab_size_(config.vocab_size),
        normalizer_(config)
    {
    }

    void BPETrainer::set_vocab_size(size_t size) {
        if (size == 0) throw TokenizerException("Vocabulary size must be positive");
        vocab_size_ = size;
        config_.vocab_size = size;
    }

    void BPETrainer::set_min_frequency(size_t freq) {
        if (freq == 0) throw TokenizerException("Minimum frequency must be at least 1");
        min_frequency_ = freq;
        config_.min_frequency = freq;
    }

    void BPETrainer::set_normalizer(const UnicodeNormalizer& norm) {
        normalizer_ = norm;
    }

    std::unordered_map<std::string, int> BPETrainer::get_word_counts(const std::vector<std::string>& corpus) {
        std::unordered_map<std::string, int> counts;
        for (const auto& text : corpus) {
            std::string normalized = normalizer_.normalize(text);
            if (config_.lowercase) {
                std::transform(normalized.begin(), normalized.end(), normalized.begin(), [](unsigned char c) { return std::tolower(c); });
            }
            std::istringstream iss(normalized);
            std::string word;
            while (iss >> word) {
                std::string word_with_suffix = word + "</w>";
                counts[word_with_suffix]++;
            }
        }
        return counts;
    }

    std::unordered_map<BPETrainer::Pair, int, BPETrainer::PairHash> BPETrainer::get_pair_counts(const std::unordered_map<std::string, int>& word_counts) const {
        std::unordered_map<Pair, int, PairHash> counts;
        for (const auto& [word, count] : word_counts) {
            std::vector<std::string> symbols;
            for (char c : word) symbols.emplace_back(1, c);

            for (size_t i = 0; i < symbols.size() - 1; ++i) {
                counts[{symbols[i], symbols[i + 1]}] += count;
            }
        }
        return counts;
    }

    void BPETrainer::merge_pair_counts(const std::string& first, const std::string& second, std::unordered_map<std::string, int>& word_counts, const Pair& best_pair, std::string& new_token) {
        std::unordered_map<std::string, int> new_word_counts;
        std::string bigram = first + second;
        std::regex pattern(first + " " + second);

        for (auto const& [word, count] : word_counts) {
            std::string new_word;
            std::string s_word = word;

            // Re-split word into symbols
            std::string current_sub;
            for (char c : s_word) {
                current_sub += c;
                if (new_word.empty()) new_word = current_sub;
                else new_word += " " + current_sub;
                current_sub = "";
            }

            std::string out_word = std::regex_replace(new_word, pattern, bigram);
            new_word_counts[out_word] = count;
        }
        word_counts = new_word_counts;
    }


    void BPETrainer::train(const std::vector<std::string>& corpus, std::shared_ptr<Vocab> vocab, size_t vocab_size) {
        if (corpus.empty()) {
            throw TokenizerException("Empty corpus provided for training");
        }
        word_counts_ = get_word_counts(corpus);

        std::unordered_map<std::string, int> initial_vocab_counts;
        for (const auto& [word, count] : word_counts_) {
            for (char c : word) {
                initial_vocab_counts[std::string(1, c)] += count;
            }
        }

        initialize_vocab(vocab, initial_vocab_counts);

        for (size_t i = vocab->size(); i < vocab_size; ++i) {
            pair_counts_ = get_pair_counts(word_counts_);
            if (pair_counts_.empty()) break;

            Pair best_pair;
            int max_freq = 0;
            for (const auto& [pair, freq] : pair_counts_) {
                if (freq > max_freq) {
                    max_freq = freq;
                    best_pair = pair;
                }
            }
            if (max_freq == 0) break;

            merge_rules_.push_back({ best_pair.first, best_pair.second });
            std::string new_token = best_pair.first + best_pair.second;
            vocab->add_token(new_token);

            std::unordered_map<std::string, int> next_word_counts;
            for (const auto& [word, count] : word_counts_) {
                std::string merged_word = word;
                size_t pos = 0;
                while ((pos = merged_word.find(best_pair.first + " " + best_pair.second, pos)) != std::string::npos) {
                    merged_word.replace(pos, best_pair.first.length() + 1 + best_pair.second.length(), new_token);
                }
                next_word_counts[merged_word] = count;
            }
            word_counts_ = next_word_counts;
        }
    }

    void BPETrainer::initialize_vocab(std::shared_ptr<Vocab> vocab, const std::unordered_map<std::string, int>& initial_vocab_counts) {
        vocab->clear();
        if (!config_.unk_token.empty()) vocab->add_special_token(config_.unk_token, SpecialTokenType::UNK);
        if (!config_.pad_token.empty()) vocab->add_special_token(config_.pad_token, SpecialTokenType::PAD);
        if (!config_.bos_token.empty()) vocab->add_special_token(config_.bos_token, SpecialTokenType::BOS);
        if (!config_.eos_token.empty()) vocab->add_special_token(config_.eos_token, SpecialTokenType::EOS);
        if (!config_.mask_token.empty()) vocab->add_special_token(config_.mask_token, SpecialTokenType::MASK);
        if (!config_.sep_token.empty()) vocab->add_special_token(config_.sep_token, SpecialTokenType::SEP);
        if (!config_.cls_token.empty()) vocab->add_special_token(config_.cls_token, SpecialTokenType::CLS);

        for (const auto& [token, count] : initial_vocab_counts) {
            if (static_cast<size_t>(count) >= min_frequency_) {
                vocab->add_token(token);
            }
        }
    }

    void BPETrainer::train_from_file(const std::string& file_path, std::shared_ptr<Vocab> vocab) {
        std::ifstream file(file_path);
        if (!file.is_open()) throw TokenizerException("Failed to open training file: " + file_path);

        std::vector<std::string> corpus;
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) corpus.push_back(line);
        }
        train(corpus, vocab, vocab_size_);
    }

    void BPETrainer::train_from_files(const std::vector<std::string>& file_paths, std::shared_ptr<Vocab> vocab) {
        if (file_paths.empty()) throw TokenizerException("No file paths provided");

        std::vector<std::string> corpus;
        for (const auto& path : file_paths) {
            std::ifstream file(path);
            if (!file.is_open()) throw TokenizerException("Failed to open file: " + path);
            std::string line;
            while (std::getline(file, line)) {
                if (!line.empty()) corpus.push_back(line);
            }
        }
        train(corpus, vocab, vocab_size_);
    }
}