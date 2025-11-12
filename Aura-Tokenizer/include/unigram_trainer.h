#pragma once

#include "tokenizer_core.h"        // For write_string / read_string
#include "vocab.h"                 // Defines auratokenizer::Vocab
#include "unicode_normalizer.h"    // Defines auratokenizer::UnicodeNormalizer
#include "tokenizer_trainer.h"     // Defines auratokenizer::TokenizerTrainerBase
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <stdexcept>
#include <cstddef>

namespace auratokenizer {

    /**
     * UnigramTrainer
     *
     * - Gathers character‐ or token‐level frequencies from a corpus.
     * - Builds a unigram vocabulary (based on min_frequency).
     * - Prunes down to `max_tokens` by selecting top‐frequency items.
     *
     * Implements TokenizerTrainerBase so it can be used interchangeably
     * with other trainer types (e.g. BPETrainer).
     */
    class UnigramTrainer : public TokenizerTrainerBase {
    public:
        UnigramTrainer();
        explicit UnigramTrainer(const TokenizerConfig& config);
        ~UnigramTrainer() override = default;

        // ─── Core training methods (override base) ───
        void train(const std::vector<std::string>& texts) override;
        void train_from_file(const std::string& file_path) override;
        void train_from_files(const std::vector<std::string>& file_paths) override;

        // ─── Save / Load model (override base) ───
        void save(const std::string& path) override;
        void load(const std::string& path) override;

        // ─── Access / modify configuration (override base) ───
        TokenizerConfig get_config() const override;
        void set_config(const TokenizerConfig& config) override;

        // ─── Unigram‐specific setters ───
        void set_min_frequency(std::size_t freq);
        void set_max_tokens(std::size_t max);
        void set_use_regex(bool use);
        void set_regex_pattern(const std::string& pattern);

        /** Return shared pointer to the constructed vocabulary. */
        std::shared_ptr<Vocab> get_vocab() const;

    private:
        // ─── Internal state ───
        TokenizerConfig          config_;
        std::size_t              min_frequency_;
        std::size_t              max_tokens_;
        bool                     use_regex_;
        std::string              regex_pattern_;
        std::shared_ptr<Vocab>   vocab_;
        UnicodeNormalizer        normalizer_;

        // ─── Helper methods ───

        /**
         * Process a single text line: normalize, tokenize, and accumulate freqcounts.
         */
        void process_text(const std::string& text,
            std::unordered_map<std::string, std::size_t>& frequencies);

        /**
         * Once all frequencies are collected, build & prune the Vocab.
         */
        void build_vocab(const std::unordered_map<std::string, std::size_t>& frequencies);

        /**
         * Simple tokenization:
         *   - If use_regex_ && regex_pattern_ is nonempty, you'd insert regex splitting.
         *   - Otherwise, default to character‐level tokens.
         */
        std::vector<std::string> tokenize_text(const std::string& text) const;
    };

} // namespace auratokenizer
