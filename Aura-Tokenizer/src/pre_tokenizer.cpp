#include "pre_tokenizer.h"

namespace auratokenizer {

    RegexPreTokenizer::RegexPreTokenizer(const TokenizerConfig& config)
        : config_(config) {
        compile_patterns();
    }

    void RegexPreTokenizer::set_config(const TokenizerConfig& config) {
        config_ = config;
        compile_patterns();
    }

    const TokenizerConfig& RegexPreTokenizer::get_config() const {
        return config_;
    }

    void RegexPreTokenizer::add_pattern(const std::string& pattern) {
        config_.pre_tokenizer_patterns.push_back(pattern);
        compile_patterns();
    }

    void RegexPreTokenizer::clear_patterns() {
        config_.pre_tokenizer_patterns.clear();
        patterns_.clear();
    }

    void RegexPreTokenizer::compile_patterns() {
        patterns_.clear();
        for (const auto& p : config_.pre_tokenizer_patterns) {
            patterns_.emplace_back(p, std::regex::ECMAScript | std::regex::optimize);
        }
    }

    std::vector<std::string> RegexPreTokenizer::pre_tokenize(const std::string& text) const {
        std::vector<std::string> tokens;
        std::string current_text = text;

        if (patterns_.empty()) {
            // Default: split by whitespace if no patterns are defined
            std::regex ws_re("\\s+");
            std::sregex_token_iterator it(current_text.begin(), current_text.end(), ws_re, -1);
            std::sregex_token_iterator end;
            for (; it != end; ++it) {
                if (!it->str().empty()) {
                    tokens.push_back(it->str());
                }
            }
        } else {
            // Apply regex patterns
            for (const auto& pattern : patterns_) {
                std::vector<std::string> temp_tokens;
                std::sregex_iterator it(current_text.begin(), current_text.end(), pattern);
                std::sregex_iterator end;
                int last_pos = 0;
                for (; it != end; ++it) {
                    if (it->position() > last_pos) {
                        temp_tokens.push_back(current_text.substr(last_pos, it->position() - last_pos));
                    }
                    temp_tokens.push_back(it->str());
                    last_pos = it->position() + it->length();
                }
                if (last_pos < current_text.length()) {
                    temp_tokens.push_back(current_text.substr(last_pos));
                }
                current_text = ""; // Reset for next pattern application
                for (const auto& t : temp_tokens) {
                    current_text += t; // Reconstruct for next pass
                }
                tokens = temp_tokens; // Update tokens for current pass
            }
        }

        // Handle special cases (e.g., splitting punctuation, etc.)
        std::vector<std::string> final_tokens;
        for (const auto& token : tokens) {
            std::vector<std::string> handled = handle_special_cases(token);
            final_tokens.insert(final_tokens.end(), handled.begin(), handled.end());
        }

        return final_tokens;
    }

    std::vector<std::vector<std::string>> RegexPreTokenizer::batch_pre_tokenize(const std::vector<std::string>& texts) const {
        std::vector<std::vector<std::string>> batch_tokens;
        batch_tokens.reserve(texts.size());
        for (const std::string& text : texts) {
            batch_tokens.push_back(pre_tokenize(text));
        }
        return batch_tokens;
    }

    std::vector<std::string> RegexPreTokenizer::handle_special_cases(const std::string& token) const {
        // This is a placeholder for more advanced pre-tokenization rules
        // such as splitting punctuation, handling numbers, etc.
        // For now, it just returns the token as is.
        return { token };
    }

} // namespace auratokenizer