#include "vector_conversions.h"

namespace auratokenizer {

std::unique_ptr<TokenVec> to_token_vec(const std::vector<Token>& tokens) {
    auto vec = std::make_unique<TokenVec>();
    vec->tokens.reserve(tokens.size());
    for (const auto& token : tokens) {
        vec->tokens.push_back(token);
    }
    return vec;
}

std::unique_ptr<IdVec> to_id_vec(const std::vector<int32_t>& ids) {
    auto vec = std::make_unique<IdVec>();
    vec->ids.reserve(ids.size());
    vec->ids = ids;
    return vec;
}

std::unique_ptr<StringVec> to_string_vec(const std::vector<std::string>& strings) {
    auto vec = std::make_unique<StringVec>();
    vec->strings.reserve(strings.size());
    for (const auto& s : strings) {
        vec->strings.push_back(s);
    }
    return vec;
}

std::unique_ptr<IdVecVec> to_id_vec_vec(const std::vector<std::vector<int32_t>>& vecs) {
    auto vec = std::make_unique<IdVecVec>();
    size_t total_size = 0;
    vec->lengths.reserve(vecs.size());
    
    // Calculate total size and store lengths
    for (const auto& v : vecs) {
        total_size += v.size();
        vec->lengths.push_back(v.size());
    }
    
    // Flatten data
    vec->data.reserve(total_size);
    for (const auto& v : vecs) {
        vec->data.insert(vec->data.end(), v.begin(), v.end());
    }
    
    return vec;
}

std::unique_ptr<StringVecVec> to_string_vec_vec(const std::vector<std::vector<std::string>>& vecs) {
    auto vec = std::make_unique<StringVecVec>();
    size_t total_size = 0;
    vec->lengths.reserve(vecs.size());
    
    // Calculate total size and store lengths
    for (const auto& v : vecs) {
        total_size += v.size();
        vec->lengths.push_back(v.size());
    }
    
    // Flatten data
    vec->data.reserve(total_size);
    for (const auto& v : vecs) {
        vec->data.insert(vec->data.end(), v.begin(), v.end());
    }
    
    return vec;
}

std::unique_ptr<MergeRuleVec> to_merge_rule_vec(const std::vector<MergeRule>& rules) {
    auto vec = std::make_unique<MergeRuleVec>();
    vec->rules.reserve(rules.size());
    for (const auto& rule : rules) {
        vec->rules.push_back(rule);
    }
    return vec;
}

std::vector<Token> from_token_vec(const TokenVec& vec) {
    return std::vector<Token>(vec.tokens.begin(), vec.tokens.end());
}

std::vector<int32_t> from_id_vec(const IdVec& vec) {
    return std::vector<int32_t>(vec.ids.begin(), vec.ids.end());
}

std::vector<std::string> from_string_vec(const StringVec& vec) {
    return std::vector<std::string>(vec.strings.begin(), vec.strings.end());
}

std::vector<std::vector<int32_t>> from_id_vec_vec(const IdVecVec& vec) {
    std::vector<std::vector<int32_t>> result;
    result.reserve(vec.lengths.size());
    
    size_t start = 0;
    for (size_t len : vec.lengths) {
        result.emplace_back(vec.data.begin() + start, vec.data.begin() + start + len);
        start += len;
    }
    
    return result;
}

std::vector<std::vector<std::string>> from_string_vec_vec(const StringVecVec& vec) {
    std::vector<std::vector<std::string>> result;
    result.reserve(vec.lengths.size());
    
    size_t start = 0;
    for (size_t len : vec.lengths) {
        result.emplace_back(vec.data.begin() + start, vec.data.begin() + start + len);
        start += len;
    }
    
    return result;
}

std::vector<MergeRule> from_merge_rule_vec(const MergeRuleVec& vec) {
    return std::vector<MergeRule>(vec.rules.begin(), vec.rules.end());
}

} // namespace auratokenizer
