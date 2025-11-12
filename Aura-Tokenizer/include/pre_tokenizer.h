#pragma once

#include <string>
#include <vector>
#include <regex>
#include "tokenizer_types.h"
#include "tokenizer_config.h"
#include "icu_utils.h"

namespace auratokenizer {

    class PreTokenizer {
    public:
        virtual ~PreTokenizer() = default;
        virtual std::vector<std::string> pre_tokenize(const std::string& text) const = 0;
        virtual std::vector<std::vector<std::string>> batch_pre_tokenize(const std::vector<std::string>& texts) const = 0;
    };

    // Concrete implementation for a regex-based pre-tokenizer (original PreTokenizer logic)
    class RegexPreTokenizer : public PreTokenizer {
    public:
        explicit RegexPreTokenizer(const TokenizerConfig& config);

        void set_config(const TokenizerConfig& config);
        const TokenizerConfig& get_config() const;

        void add_pattern(const std::string& pattern);
        void clear_patterns();

        std::vector<std::string> pre_tokenize(const std::string& text) const override;
        std::vector<std::vector<std::string>> batch_pre_tokenize(const std::vector<std::string>& texts) const override;

    private:
        void compile_patterns();
        std::vector<std::string> split_by_patterns(const std::string& text) const;
        std::vector<std::string> handle_special_cases(const std::string& token) const;

        TokenizerConfig config_;
        std::vector<std::regex> patterns_;
    };

} // namespace auratokenizer