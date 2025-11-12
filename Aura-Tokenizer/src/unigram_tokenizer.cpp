#include "unigram_tokenizer.h"
#include "tokenizer_exception.h"
#include <algorithm>
#include <limits>

namespace auratokenizer {

UnigramTokenizer::UnigramTokenizer(const TokenizerConfig& config)
    : TokenizerBase(config),
      normalizer_(config),
      vocab_(std::make_shared<Vocab>())
{
    initialize_special_tokens();
}

UnigramTokenizer::~UnigramTokenizer() = default;

void UnigramTokenizer::initialize_special_tokens() {
    // Populate special_tokens_map_ from config
    if (!config_.unk_token.empty()) {
        vocab_->add_special_token(config_.unk_token, SpecialTokenType::UNK);
        special_tokens_[SpecialTokenType::UNK] = config_.unk_token;
    }
    if (!config_.pad_token.empty()) {
        vocab_->add_special_token(config_.pad_token, SpecialTokenType::PAD);
        special_tokens_[SpecialTokenType::PAD] = config_.pad_token;
    }
    if (!config_.bos_token.empty()) {
        vocab_->add_special_token(config_.bos_token, SpecialTokenType::BOS);
        special_tokens_[SpecialTokenType::BOS] = config_.bos_token;
    }
    if (!config_.eos_token.empty()) {
        vocab_->add_special_token(config_.eos_token, SpecialTokenType::EOS);
        special_tokens_[SpecialTokenType::EOS] = config_.eos_token;
    }
    if (!config_.mask_token.empty()) {
        vocab_->add_special_token(config_.mask_token, SpecialTokenType::MASK);
        special_tokens_[SpecialTokenType::MASK] = config_.mask_token;
    }
    if (!config_.sep_token.empty()) {
        vocab_->add_special_token(config_.sep_token, SpecialTokenType::SEP);
        special_tokens_[SpecialTokenType::SEP] = config_.sep_token;
    }
    if (!config_.cls_token.empty()) {
        vocab_->add_special_token(config_.cls_token, SpecialTokenType::CLS);
        special_tokens_[SpecialTokenType::CLS] = config_.cls_token;
    }

    // Add any custom added tokens from config
    for (const auto& pair : config_.added_tokens) {
        vocab_->add_special_token(pair.first, SpecialTokenType::CUSTOM);
    }
}

std::vector<std::string> UnigramTokenizer::pre_tokenize(const std::string& text) const {
    // For Unigram, typically pre-tokenize by characters or simple whitespace
    // For now, a simple character-level pre-tokenization
    std::vector<std::string> result;
    for (char c : text) {
        result.push_back(std::string(1, c));
    }
    return result;
}

std::vector<Token> UnigramTokenizer::encode(const std::string& text) {
    std::string normalized_text = normalizer_.normalize(text);
    std::vector<std::string> segments = viterbi_segment(normalized_text);

    std::vector<Token> tokens;
    for (const auto& segment : segments) {
        int id = vocab_->get_token_id(segment);
        tokens.emplace_back(id, segment, vocab_->is_special_token(segment));
    }
    return tokens;
}

std::vector<int> UnigramTokenizer::encode_to_ids(const std::string& text) {
    std::string normalized_text = normalizer_.normalize(text);
    std::vector<std::string> segments = viterbi_segment(normalized_text);

    std::vector<int> ids;
    for (const auto& segment : segments) {
        ids.push_back(vocab_->get_token_id(segment));
    }
    return ids;
}

std::string UnigramTokenizer::decode(const std::vector<Token>& tokens) {
    std::string decoded_text;
    for (const auto& token : tokens) {
        decoded_text += token.text;
    }
    return decoded_text;
}

std::string UnigramTokenizer::decode_from_ids(const std::vector<int>& ids) {
    std::string decoded_text;
    for (int id : ids) {
        decoded_text += vocab_->get_token(id);
    }
    return decoded_text;
}

std::vector<std::vector<int>> UnigramTokenizer::batch_encode(const std::vector<std::string>& texts) {
    std::vector<std::vector<int>> batch_ids;
    batch_ids.reserve(texts.size());
    for (const auto& text : texts) {
        batch_ids.push_back(encode_to_ids(text));
    }
    return batch_ids;
}

std::vector<std::string> UnigramTokenizer::batch_decode(const std::vector<std::vector<int>>& ids_batch) {
    std::vector<std::string> batch_decoded;
    batch_decoded.reserve(ids_batch.size());
    for (const auto& ids : ids_batch) {
        batch_decoded.push_back(decode_from_ids(ids));
    }
    return batch_decoded;
}

void UnigramTokenizer::save(const std::string& path) {
    throw TokenizerException("UnigramTokenizer::save not implemented yet.");
}

void UnigramTokenizer::load(const std::string& path) {
    throw TokenizerException("UnigramTokenizer::load not implemented yet.");
}

void UnigramTokenizer::train(const std::vector<std::string>& corpus, size_t vocab_size) {
    throw TokenizerException("UnigramTokenizer::train not implemented yet. Use UnigramTrainer.");
}

void UnigramTokenizer::add_special_tokens(const std::vector<std::string>& tokens) {
    for (const auto& token : tokens) {
        vocab_->add_special_token(token, SpecialTokenType::CUSTOM);
    }
}

std::vector<std::string> UnigramTokenizer::get_special_tokens() const {
    std::vector<std::string> special_tokens_list;
    for (const auto& pair : special_tokens_) {
        special_tokens_list.push_back(pair.second);
    }
    return special_tokens_list;
}

bool UnigramTokenizer::is_special_token(const std::string& token) const {
    return vocab_->is_special_token(token);
}

int UnigramTokenizer::get_special_token_id(SpecialTokenType type) const {
    auto it = special_tokens_.find(type);
    if (it != special_tokens_.end()) {
        return vocab_->get_token_id(it->second);
    }
    return -1;
}

const TokenizerConfig& UnigramTokenizer::get_config() const {
    return config_;
}

void UnigramTokenizer::set_config(const TokenizerConfig& config) {
    config_ = config;
    normalizer_.set_config(config_);
    initialize_special_tokens();
}

void UnigramTokenizer::set_vocab(std::shared_ptr<Vocab> vocab) {
    vocab_ = vocab;
    initialize_special_tokens();
}

void UnigramTokenizer::set_vocab_and_scores(std::shared_ptr<Vocab> vocab, const std::unordered_map<std::string, float>& scores) {
    vocab_ = vocab;
    scores_ = scores;
}

std::vector<std::string> UnigramTokenizer::viterbi_segment(const std::string& text) const {
    // This is a simplified Viterbi algorithm for demonstration.
    // A full implementation would be more complex, involving dynamic programming
    // to find the best segmentation based on token scores.

    if (text.empty()) return {};

    std::vector<std::string> result;
    std::string current_segment;

    for (char c : text) {
        current_segment += c;
        // Check if current_segment is in vocab
        if (vocab_->contains_token(current_segment)) {
            // If it is, and it's a good candidate, add it and reset
            // This logic needs to be more sophisticated for actual Unigram
            result.push_back(current_segment);
            current_segment.clear();
        }
    }
    // Add any remaining segment
    if (!current_segment.empty()) {
        result.push_back(current_segment);
    }

    // Fallback: if no segmentation found, return character by character
    if (result.empty() && !text.empty()) {
        for (char c : text) {
            result.push_back(std::string(1, c));
        }
    }

    return result;
}

} // namespace auratokenizer
