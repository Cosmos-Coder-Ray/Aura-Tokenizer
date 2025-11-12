#include "tokenizer_model.h"

namespace auratokenizer {

TokenizerModel::TokenizerModel()
    : algorithm_(TokenizationAlgorithm::BPE) // Default to BPE
{
}

void TokenizerModel::add_token_to_vocab(const std::string& token, int id) {
    vocab_[token] = id;
}

void TokenizerModel::add_token_with_score(const std::string& token, float score) {
    scores_[token] = score;
}

void TokenizerModel::add_merge(const std::string& merge_str) {
    merges_.push_back(merge_str);
}

} // namespace auratokenizer