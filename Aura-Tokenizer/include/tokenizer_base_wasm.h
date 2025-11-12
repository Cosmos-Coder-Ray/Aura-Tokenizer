#pragma once

#include "tokenizer_types.h"
#include "token.h"
#include "utf8_utils.h"
#include "icu_utils.h"
#include <string>
#include <vector>

namespace auratokenizer {

    class TokenizerBaseWASM {
    public:
        virtual ~TokenizerBaseWASM() = default;

        virtual std::vector<Token> encode(const std::string& text) { return {}; }
        virtual std::vector<int> encode_to_ids(const std::string& text) { return {}; }
        virtual std::string decode(const std::vector<Token>& tokens) { return ""; }
        virtual std::string decode_from_ids(const std::vector<int>& ids) { return ""; }
        virtual void train(const std::vector<std::string>& texts, size_t vocab_size) {}
        virtual void save(const std::string& path) {}
        virtual void load(const std::string& path) {}
        virtual void add_special_tokens(const std::vector<std::string>& tokens) {}
        virtual std::vector<std::string> get_special_tokens() const { return {}; }
        virtual bool is_special_token(const std::string& token) const { return false; }
        virtual TokenizerConfig get_config() const { return {}; }
        virtual void set_config(const TokenizerConfig& config) {}
    };

}