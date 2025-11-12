#pragma once

#include "tokenizer_core.h"
#include "vocab.h"
#include "unicode_normalizer.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

namespace auratokenizer {

class UnigramTokenizer : public TokenizerBase {
public:
    explicit UnigramTokenizer(const TokenizerConfig& config = TokenizerConfig());
    ~UnigramTokenizer() override;

    std::vector<Token> encode(const std::string& text) override;
    std::vector<int> encode_to_ids(const std::string& text) override;

    std::string decode(const std::vector<Token>& tokens) override;
    std::string decode_from_ids(const std::vector<int>& ids) override;

    std::vector<std::vector<int>> batch_encode(const std::vector<std::string>& texts) override;
    std::vector<std::string> batch_decode(const std::vector<std::vector<int>>& ids_batch) override;

    void save(const std::string& path) override;
    void load(const std::string& path) override;

    void train(const std::vector<std::string>& corpus, size_t vocab_size) override;

    void add_special_tokens(const std::vector<std::string>& tokens) override;
    std::vector<std::string> get_special_tokens() const override;
    bool is_special_token(const std::string& token) const override;
    int get_special_token_id(SpecialTokenType type) const;

    const TokenizerConfig& get_config() const override;
    void set_config(const TokenizerConfig& config) override;
    void set_vocab(std::shared_ptr<Vocab> vocab) override;

    // Unigram-specific methods
    void set_vocab_and_scores(std::shared_ptr<Vocab> vocab, const std::unordered_map<std::string, float>& scores);

private:
    UnicodeNormalizer normalizer_;
    std::shared_ptr<Vocab> vocab_;
    std::unordered_map<SpecialTokenType, std::string> special_tokens_;
    std::unordered_map<std::string, float> scores_; // Token scores for Unigram

    void initialize_special_tokens();
    std::vector<std::string> pre_tokenize(const std::string& text) const;
    std::vector<Token> encode_unigram(const std::string& word) const;
    std::vector<std::string> viterbi_segment(const std::string& text) const;
};

} // namespace auratokenizer
