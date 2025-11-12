#pragma once

#include "pre_tokenizer.h"
#include "tokenizer_config.h"
#include "icu_utils.h"

namespace auratokenizer {

    class ByteLevelPreTokenizer : public PreTokenizer {
    public:
        explicit ByteLevelPreTokenizer(const TokenizerConfig& config);

        std::vector<std::string> pre_tokenize(const std::string& text) const override;
        std::vector<std::vector<std::string>> batch_pre_tokenize(const std::vector<std::string>& texts) const override;

    private:
        TokenizerConfig config_;
    };

} // namespace auratokenizer