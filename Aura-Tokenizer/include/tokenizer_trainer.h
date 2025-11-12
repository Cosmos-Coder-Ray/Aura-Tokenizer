#pragma once

#include "tokenizer_config.h"
#include "vocab.h"
#include <string>
#include <vector>
#include <memory>

namespace auratokenizer {

    /**
     * TokenizerTrainerBase
     *
     * Abstract base class for tokenizer trainers.
     * Provides a common interface for different training algorithms
     * like BPE, Unigram, etc.
     */
    class TokenizerTrainerBase {
    public:
        TokenizerTrainerBase() = default;
        virtual ~TokenizerTrainerBase() = default;

        // Core training methods
        virtual void train(const std::vector<std::string>& texts) = 0;
        virtual void train_from_file(const std::string& file_path) = 0;
        virtual void train_from_files(const std::vector<std::string>& file_paths) = 0;

        // Save / Load model
        virtual void save(const std::string& path) = 0;
        virtual void load(const std::string& path) = 0;

        // Configuration
        virtual TokenizerConfig get_config() const = 0;
        virtual void set_config(const TokenizerConfig& config) = 0;

        // Get the trained vocabulary
        virtual std::shared_ptr<Vocab> get_vocab() const = 0;

    protected:
        TokenizerConfig config_;
    };

} // namespace auratokenizer 