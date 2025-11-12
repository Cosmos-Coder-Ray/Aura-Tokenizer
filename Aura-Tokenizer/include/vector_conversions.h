#pragma once

#include "auratokenizer_c_api.h"
#include "token.h"
#include "rust/cxx.h"
#include <memory>
#include <vector>

namespace auratokenizer {

// Convert from C++ vectors to Rust FFI types
std::unique_ptr<TokenVec> to_token_vec(const std::vector<Token>& tokens);
std::unique_ptr<IdVec> to_id_vec(const std::vector<int32_t>& ids);
std::unique_ptr<StringVec> to_string_vec(const std::vector<std::string>& strings);
std::unique_ptr<IdVecVec> to_id_vec_vec(const std::vector<std::vector<int32_t>>& vecs);
std::unique_ptr<StringVecVec> to_string_vec_vec(const std::vector<std::vector<std::string>>& vecs);
std::unique_ptr<MergeRuleVec> to_merge_rule_vec(const std::vector<MergeRule>& rules);

// Convert from Rust FFI types to C++ vectors
std::vector<Token> from_token_vec(const TokenVec& vec);
std::vector<int32_t> from_id_vec(const IdVec& vec);
std::vector<std::string> from_string_vec(const StringVec& vec);
std::vector<std::vector<int32_t>> from_id_vec_vec(const IdVecVec& vec);
std::vector<std::vector<std::string>> from_string_vec_vec(const StringVecVec& vec);
std::vector<MergeRule> from_merge_rule_vec(const MergeRuleVec& vec);

} // namespace auratokenizer
