#include "wordpiece_model.h"
#include <algorithm>
#include <iostream>

namespace auratokenizer {

namespace models {

void WordPieceModel::initialize(
    const std::unordered_map<std::string, int>& vocab,
    const std::string& unk_token,
    int max_input_chars_per_word
) {
    vocab_ = vocab;
    unk_token_ = unk_token;
    max_input_chars_per_word_ = max_input_chars_per_word;
}

std::vector<std::string> WordPieceModel::tokenize(const std::string& word) const {
    std::vector<std::string> output_tokens;
    if (word.empty()) {
        return output_tokens;
    }

    std::string current_word = word;
    int start = 0;
    while (start < current_word.length()) {
        int end = current_word.length();
        std::string current_chunk;
        bool found_in_vocab = false;
        while (start < end) {
            std::string chunk = current_word.substr(start, end - start);
            if (start > 0) {
                chunk = "##" + chunk;
            }
            if (vocab_.count(chunk)) {
                current_chunk = chunk;
                found_in_vocab = true;
                break;
            }
            end--;
        }
        if (found_in_vocab) {
            output_tokens.push_back(current_chunk);
            start = end;
        } else {
            output_tokens.push_back(unk_token_);
            break; // If a chunk is not found, the rest of the word is unknown
        }
    }
    return output_tokens;
}

std::vector<std::vector<std::string>> WordPieceModel::batch_tokenize(const std::vector<std::string>& words) const {
    std::vector<std::vector<std::string>> batch_tokens;
    batch_tokens.reserve(words.size());
    for (const std::string& word : words) {
        batch_tokens.push_back(tokenize(word));
    }
    return batch_tokens;
}

} // namespace models

} // namespace auratokenizer
