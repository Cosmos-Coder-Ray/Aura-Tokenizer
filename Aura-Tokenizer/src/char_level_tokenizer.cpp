#include "char_level_tokenizer.h"
#include "tokenizer_exception.h"

namespace auratokenizer {

CharLevelTokenizer::CharLevelTokenizer(const TokenizerConfig& config)
    : TokenizerBase(config),
      normalizer_(config),
      vocab_(std::make_shared<Vocab>())
{
    initialize_special_tokens();
}

CharLevelTokenizer::~CharLevelTokenizer() = default;

void CharLevelTokenizer::initialize_special_tokens() {
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

std::vector<Token> CharLevelTokenizer::encode(const std::string& text) {
    std::string normalized_text = normalizer_.normalize(text);
    std::vector<Token> tokens;
    for (char c : normalized_text) {
        std::string s(1, c);
        tokens.emplace_back(vocab_->get_token_id(s), s, vocab_->is_special_token(s));
    }
    return tokens;
}

std::vector<int> CharLevelTokenizer::encode_to_ids(const std::string& text) {
    std::string normalized_text = normalizer_.normalize(text);
    std::vector<int> ids;
    for (char c : normalized_text) {
        ids.push_back(vocab_->get_token_id(std::string(1, c)));
    }
    return ids;
}

std::string CharLevelTokenizer::decode(const std::vector<Token>& tokens) {
    std::string decoded_text;
    for (const auto& token : tokens) {
        decoded_text += token.text;
    }
    return decoded_text;
}

std::string CharLevelTokenizer::decode_from_ids(const std::vector<int>& ids) {
    std::string decoded_text;
    for (int id : ids) {
        decoded_text += vocab_->get_token(id);
    }
    return decoded_text;
}

std::vector<std::vector<int>> CharLevelTokenizer::batch_encode(const std::vector<std::string>& texts) {
    std::vector<std::vector<int>> batch_ids;
    batch_ids.reserve(texts.size());
    for (const auto& text : texts) {
        batch_ids.push_back(encode_to_ids(text));
    }
    return batch_ids;
}

std::vector<std::string> CharLevelTokenizer::batch_decode(const std::vector<std::vector<int>>& ids_batch) {
    std::vector<std::string> batch_decoded;
    batch_decoded.reserve(ids_batch.size());
    for (const auto& ids : ids_batch) {
        batch_decoded.push_back(decode_from_ids(ids));
    }
    return batch_decoded;
}

void CharLevelTokenizer::save(const std::string& path) {
    throw TokenizerException("CharLevelTokenizer::save not implemented yet.");
}

void CharLevelTokenizer::load(const std::string& path) {
    throw TokenizerException("CharLevelTokenizer::load not implemented yet.");
}

void CharLevelTokenizer::train(const std::vector<std::string>& corpus, size_t vocab_size) {
    // For character-level, training is simply building a vocabulary of all unique characters
    // encountered in the corpus.
    vocab_->clear();
    int id_counter = 0;
    for (const std::string& text : corpus) {
        std::string normalized_text = normalizer_.normalize(text);
        for (char c : normalized_text) {
            std::string s(1, c);
            if (!vocab_->contains_token(s)) {
                vocab_->add_token_to_vocab(s, id_counter++);
            }
        }
    }
    // Add special tokens after corpus characters
    initialize_special_tokens();
}

void CharLevelTokenizer::add_special_tokens(const std::vector<std::string>& tokens) {
    for (const auto& token : tokens) {
        vocab_->add_special_token(token, SpecialTokenType::CUSTOM);
    }
}

std::vector<std::string> CharLevelTokenizer::get_special_tokens() const {
    std::vector<std::string> special_tokens_list;
    for (const auto& pair : special_tokens_) {
        special_tokens_list.push_back(pair.second);
    }
    return special_tokens_list;
}

bool CharLevelTokenizer::is_special_token(const std::string& token) const {
    return vocab_->is_special_token(token);
}

int CharLevelTokenizer::get_special_token_id(SpecialTokenType type) const {
    auto it = special_tokens_.find(type);
    if (it != special_tokens_.end()) {
        return vocab_->get_token_id(it->second);
    }
    return -1;
}

const TokenizerConfig& CharLevelTokenizer::get_config() const {
    return config_;
}

void CharLevelTokenizer::set_config(const TokenizerConfig& config) {
    config_ = config;
    normalizer_.set_config(config_);
    initialize_special_tokens();
}

void CharLevelTokenizer::set_vocab(std::shared_ptr<Vocab> vocab) {
    vocab_ = vocab;
    initialize_special_tokens();
}

} // namespace auratokenizer
