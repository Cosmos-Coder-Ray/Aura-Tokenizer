#include "wordpiece_tokenizer.h"
#include "tokenizer_exception.h"

namespace auratokenizer {

WordPieceTokenizer::WordPieceTokenizer(const TokenizerConfig& config)
    : TokenizerBase(config),
      normalizer_(config),
      vocab_(std::make_shared<Vocab>())
{
    initialize_special_tokens();
}

WordPieceTokenizer::~WordPieceTokenizer() = default;

void WordPieceTokenizer::initialize_special_tokens() {
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

std::vector<Token> WordPieceTokenizer::encode(const std::string& text) {
    if (!wordpiece_model_) {
        throw TokenizerException("WordPieceModel not set for WordPieceTokenizer.");
    }
    std::string normalized_text = normalizer_.normalize(text);
    // WordPiece typically works on pre-tokenized words (e.g., whitespace separated)
    // For simplicity, we'll treat the whole normalized_text as one word for now.
    // A proper implementation would use a pre-tokenizer here.
    std::vector<std::string> word_pieces = wordpiece_model_->tokenize(normalized_text);

    std::vector<Token> tokens;
    for (const auto& piece : word_pieces) {
        int id = vocab_->get_token_id(piece);
        tokens.emplace_back(id, piece, vocab_->is_special_token(piece));
    }
    return tokens;
}

std::vector<int> WordPieceTokenizer::encode_to_ids(const std::string& text) {
    if (!wordpiece_model_) {
        throw TokenizerException("WordPieceModel not set for WordPieceTokenizer.");
    }
    std::string normalized_text = normalizer_.normalize(text);
    std::vector<std::string> word_pieces = wordpiece_model_->tokenize(normalized_text);

    std::vector<int> ids;
    for (const auto& piece : word_pieces) {
        ids.push_back(vocab_->get_token_id(piece));
    }
    return ids;
}

std::string WordPieceTokenizer::decode(const std::vector<Token>& tokens) {
    std::string decoded_text;
    for (const auto& token : tokens) {
        // Remove ## for decoding
        std::string piece = token.text;
        if (piece.rfind("##", 0) == 0) {
            piece = piece.substr(2);
        }
        decoded_text += piece;
    }
    return decoded_text;
}

std::string WordPieceTokenizer::decode_from_ids(const std::vector<int>& ids) {
    std::string decoded_text;
    for (int id : ids) {
        std::string piece = vocab_->get_token(id);
        // Remove ## for decoding
        if (piece.rfind("##", 0) == 0) {
            piece = piece.substr(2);
        }
        decoded_text += piece;
    }
    return decoded_text;
}

std::vector<std::vector<int>> WordPieceTokenizer::batch_encode(const std::vector<std::string>& texts) {
    std::vector<std::vector<int>> batch_ids;
    batch_ids.reserve(texts.size());
    for (const auto& text : texts) {
        batch_ids.push_back(encode_to_ids(text));
    }
    return batch_ids;
}

std::vector<std::string> WordPieceTokenizer::batch_decode(const std::vector<std::vector<int>>& ids_batch) {
    std::vector<std::string> batch_decoded;
    batch_decoded.reserve(ids_batch.size());
    for (const auto& ids : ids_batch) {
        batch_decoded.push_back(decode_from_ids(ids));
    }
    return batch_decoded;
}

void WordPieceTokenizer::save(const std::string& path) {
    throw TokenizerException("WordPieceTokenizer::save not implemented yet.");
}

void WordPieceTokenizer::load(const std::string& path) {
    throw TokenizerException("WordPieceTokenizer::load not implemented yet.");
}

void WordPieceTokenizer::train(const std::vector<std::string>& corpus, size_t vocab_size) {
    throw TokenizerException("WordPieceTokenizer::train not implemented yet.");
}

void WordPieceTokenizer::add_special_tokens(const std::vector<std::string>& tokens) {
    for (const auto& token : tokens) {
        vocab_->add_special_token(token, SpecialTokenType::CUSTOM);
    }
}

std::vector<std::string> WordPieceTokenizer::get_special_tokens() const {
    std::vector<std::string> special_tokens_list;
    for (const auto& pair : special_tokens_) {
        special_tokens_list.push_back(pair.second);
    }
    return special_tokens_list;
}

bool WordPieceTokenizer::is_special_token(const std::string& token) const {
    return vocab_->is_special_token(token);
}

int WordPieceTokenizer::get_special_token_id(SpecialTokenType type) const {
    auto it = special_tokens_.find(type);
    if (it != special_tokens_.end()) {
        return vocab_->get_token_id(it->second);
    }
    return -1;
}

const TokenizerConfig& WordPieceTokenizer::get_config() const {
    return config_;
}

void WordPieceTokenizer::set_config(const TokenizerConfig& config) {
    config_ = config;
    normalizer_.set_config(config_);
    initialize_special_tokens();
}

void WordPieceTokenizer::set_vocab(std::shared_ptr<Vocab> vocab) {
    vocab_ = vocab;
    initialize_special_tokens();
}

void WordPieceTokenizer::set_wordpiece_model(std::shared_ptr<models::WordPieceModel> model) {
    wordpiece_model_ = model;
    // Update vocab with WordPiece model's vocab
    if (wordpiece_model_) {
        for (const auto& pair : wordpiece_model_->get_vocab()) {
            vocab_->add_token_to_vocab(pair.first, pair.second);
        }
    }
}

} // namespace auratokenizer
