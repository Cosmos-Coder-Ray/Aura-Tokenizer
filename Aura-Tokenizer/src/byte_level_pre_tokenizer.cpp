#include "byte_level_pre_tokenizer.h"
#include <vector>
#include <string>

namespace auratokenizer {

ByteLevelPreTokenizer::ByteLevelPreTokenizer(const TokenizerConfig& config) : config_(config) {}

std::vector<std::string> ByteLevelPreTokenizer::pre_tokenize(const std::string& text) const {
    std::vector<std::string> tokens;
    tokens.reserve(text.length()); // Reserve space, assuming 1 byte per char for ASCII, but will reallocate for UTF-8
    for (unsigned char byte : text) {
        tokens.push_back(std::string(1, static_cast<char>(byte)));
    }
    return tokens;
}

std::vector<std::vector<std::string>> ByteLevelPreTokenizer::batch_pre_tokenize(const std::vector<std::string>& texts) const {
    std::vector<std::vector<std::string>> batch_tokens;
    batch_tokens.reserve(texts.size());
    for (const std::string& text : texts) {
        batch_tokens.push_back(pre_tokenize(text));
    }
    return batch_tokens;
}

} // namespace auratokenizer
