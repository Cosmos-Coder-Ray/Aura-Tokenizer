#pragma once

#include "tokenizer_core.h"
#include "vocab.h"
#include <memory>
#include <string>
#include <vector>

namespace auratokenizer {

    class PythonTokenizer {
    public:
        PythonTokenizer();
        ~PythonTokenizer() = default;

        void set_vocab(std::shared_ptr<Vocab> vocab) { vocab_ = vocab; }
        std::shared_ptr<Vocab> get_vocab() const { return vocab_; }

        void set_config(const TokenizerConfig& config) { config_ = config; }
        const TokenizerConfig& get_config() const { return config_; }

        std::vector<int> encode(const std::string& text) const;
        std::string decode(const std::vector<int>& ids) const;

        std::vector<std::vector<int>> encode_batch(const std::vector<std::string>& texts) const;
        std::vector<std::string> decode_batch(const std::vector<std::vector<int>>& ids_batch) const;

    private:
        std::shared_ptr<Vocab> vocab_;
        TokenizerConfig config_;
    };

} // namespace auratokenizer
