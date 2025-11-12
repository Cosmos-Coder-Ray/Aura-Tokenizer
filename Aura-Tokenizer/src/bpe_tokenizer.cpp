#include "bpe_tokenizer.h"
#include "serialization_utils.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace auratokenizer {

    BPETokenizer::BPETokenizer(const TokenizerConfig& config)
        : TokenizerBase(config),
          normalizer_(config),
          vocab_(std::make_shared<Vocab>()) {
        initialize_special_tokens();
    }

    BPETokenizer::~BPETokenizer() = default;

    void BPETokenizer::initialize_special_tokens() {
        special_tokens_.clear();
        vocab_->clear();
        auto add = [&](SpecialTokenType type, const std::string& text) {
            if (!text.empty()) {
                vocab_->add_special_token(text, type);
                special_tokens_[type] = text;
            }
            };
        add(SpecialTokenType::UNK, config_.unk_token);
        add(SpecialTokenType::PAD, config_.pad_token);
        add(SpecialTokenType::BOS, config_.bos_token);
        add(SpecialTokenType::EOS, config_.eos_token);
        add(SpecialTokenType::MASK, config_.mask_token);
        add(SpecialTokenType::SEP, config_.sep_token);
        add(SpecialTokenType::CLS, config_.cls_token);
    }

    void BPETokenizer::set_config(const TokenizerConfig& config) {
        config_ = config;
        normalizer_ = UnicodeNormalizer(config_);
        initialize_special_tokens();
    }

    std::vector<Token> BPETokenizer::encode(const std::string& text) {
        std::vector<std::string> words = pre_tokenize(text);
        std::vector<Token> tokens;
        for (const auto& word : words) {
            auto sub_tokens = encode_bpe(word);
            tokens.insert(tokens.end(), sub_tokens.begin(), sub_tokens.end());
        }
        return tokens;
    }

    std::vector<int> BPETokenizer::encode_to_ids(const std::string& text) {
        std::vector<Token> token_objs = encode(text);
        std::vector<int> ids;
        ids.reserve(token_objs.size());
        for (const auto& t : token_objs) {
            ids.push_back(t.id);
        }
        return ids;
    }

    std::vector<std::vector<int>> BPETokenizer::batch_encode(const std::vector<std::string>& texts) {
        std::vector<std::vector<int>> batch_ids;
        batch_ids.reserve(texts.size());
        for (const auto& text : texts) {
            batch_ids.push_back(encode_to_ids(text));
        }
        return batch_ids;
    }

    std::string BPETokenizer::decode(const std::vector<Token>& tokens) {
        std::string result;
        for (const auto& t : tokens) {
            result += t.text;
        }
        return post_process_text(result);
    }

    std::string BPETokenizer::decode_from_ids(const std::vector<int>& ids) {
        std::string result;
        for (int id : ids) {
            result += vocab_->get_token(id);
        }
        return post_process_text(result);
    }

    std::vector<std::string> BPETokenizer::batch_decode(const std::vector<std::vector<int>>& ids_batch) {
        std::vector<std::string> results;
        results.reserve(ids_batch.size());
        for (const auto& ids : ids_batch) {
            results.push_back(decode_from_ids(ids));
        }
        return results;
    }

    void BPETokenizer::train(const std::vector<std::string>& corpus, size_t vocab_size) {
        if (corpus.empty()) {
            throw TokenizerException("Empty corpus provided for training");
        }
        BPETrainer trainer(config_);
        trainer.train(corpus, vocab_, vocab_size);
        merge_rules_ = trainer.get_merge_rules();
        build_merge_ranks();
    }

    void BPETokenizer::save(const std::string& path) {
        std::ofstream ofs(path, std::ios::binary);
        if (!ofs) { throw TokenizerException("Failed to open file for writing: " + path); }
        config_.save(ofs);
        vocab_->save(ofs);

        size_t rules_count = merge_rules_.size();
        write_primitive(ofs, &rules_count, sizeof(rules_count));
        for (const auto& rule : merge_rules_) {
            write_string(ofs, rule.first);
            write_string(ofs, rule.second);
        }
    }

    void BPETokenizer::load(const std::string& path) {
        std::ifstream ifs(path, std::ios::binary);
        if (!ifs) { throw TokenizerException("Failed to open file for reading: " + path); }

        config_.load(ifs);
        set_config(config_); // Re-initialize with loaded config

        vocab_->load(ifs); // Vocab is loaded after config sets up special tokens

        size_t rules_count;
        read_primitive(ifs, &rules_count, sizeof(rules_count));
        merge_rules_.clear();
        merge_rules_.reserve(static_cast<size_t>(rules_count));
        for (size_t i = 0; i < rules_count; ++i) {
            std::string first = read_string(ifs);
            std::string second = read_string(ifs);
            merge_rules_.emplace_back(first, second);
        }
        build_merge_ranks();
    }

    void BPETokenizer::add_special_tokens(const std::vector<std::string>& tokens) {
        for (const auto& token : tokens) {
            if (!token.empty() && !vocab_->has_token(token)) {
                vocab_->add_special_token(token, SpecialTokenType::CUSTOM);
            }
        }
    }

    std::vector<std::string> BPETokenizer::get_special_tokens() const { return vocab_->get_special_tokens(); }
    bool BPETokenizer::is_special_token(const std::string& token) const { return vocab_->is_special_token(token); }
    int BPETokenizer::get_special_token_id(SpecialTokenType type) const { return vocab_->get_special_token_id(type); }
    const TokenizerConfig& BPETokenizer::get_config() const { return config_; }
    void BPETokenizer::set_vocab(std::shared_ptr<Vocab> vocab) {
        vocab_ = vocab;
        initialize_special_tokens();
    }
    const std::vector<std::pair<std::string, std::string>>& BPETokenizer::get_merge_rules() const { return merge_rules_; }

    void BPETokenizer::set_merge_rules(const std::vector<std::string>& merges) {
        merge_rules_.clear();
        for (const auto& merge_str : merges) {
            // Assuming merge_str is in format "token1 token2"
            size_t space_pos = merge_str.find(' ');
            if (space_pos != std::string::npos) {
                merge_rules_.emplace_back(merge_str.substr(0, space_pos), merge_str.substr(space_pos + 1));
            }
        }
        build_merge_ranks();
    }

    std::vector<std::string> BPETokenizer::pre_tokenize(const std::string& text) const {
        std::string normalized = normalizer_.normalize(text);
        // This is a simple whitespace splitter. A real implementation might use ICU.
        std::vector<std::string> words;
        std::stringstream ss(normalized);
        std::string word;
        while (ss >> word) {
            words.push_back(word);
        }
        return words;
    }

    std::vector<Token> BPETokenizer::encode_bpe(const std::string& word) const {
        if (word.empty()) return {};

        std::vector<std::string> parts;
        for (char c : word) parts.emplace_back(1, c);

        while (parts.size() > 1) {
            int best_rank = -1;
            int merge_idx = -1;
            for (size_t i = 0; i < parts.size() - 1; ++i) {
                std::string pair_key = parts[i] + " " + parts[i + 1];
                auto it = merge_ranks_.find(pair_key);
                if (it != merge_ranks_.end()) {
                    if (best_rank == -1 || it->second < best_rank) {
                        best_rank = it->second;
                        merge_idx = i;
                    }
                }
            }
            if (merge_idx == -1) break;

            parts[merge_idx] += parts[merge_idx + 1];
            parts.erase(parts.begin() + merge_idx + 1);
        }

        std::vector<Token> result;
        int unk_id = vocab_->get_special_token_id(SpecialTokenType::UNK);
        for (const auto& p : parts) {
            int id = vocab_->get_token_id(p);
            result.emplace_back(id >= 0 ? id : unk_id, p, vocab_->is_special_token(p));
        }
        return result;
    }

    void BPETokenizer::build_merge_ranks() {
        merge_ranks_.clear();
        for (size_t i = 0; i < merge_rules_.size(); ++i) {
            merge_ranks_[merge_rules_[i].first + " " + merge_rules_[i].second] = i;
        }
    }

    std::string BPETokenizer::post_process_text(const std::string& text) const {
        // Simple placeholder for post-processing
        return text;
    }

}