#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "tokenizer_config.h"
#include "tokenizer_model.h"
#include "pre_tokenizer.h"
#include "post_processor.h"
#include "unicode_normalizer.h"
#include "nlohmann/json.hpp"

namespace auratokenizer {

class TokenizerJsonParser {
public:
    /**
     * Parses a tokenizer.json file and configures the tokenizer.
     * @param json_file_path The path to the tokenizer.json file.
     * @param config The TokenizerConfig object to populate.
     * @param model The TokenizerModel object to populate.
     * @param pre_tokenizer The PreTokenizer object to configure.
     * @param post_processor The PostProcessor object to configure.
     * @param normalizer The UnicodeNormalizer object to configure.
     */
    static void parse(
        const std::string& json_content,
        TokenizerConfig& config,
        TokenizerModel& model,
        std::shared_ptr<PreTokenizer>& pre_tokenizer,
        std::shared_ptr<PostProcessor>& post_processor,
        UnicodeNormalizer& normalizer
    );

private:
    static void parse_model(const nlohmann::json& json_model, TokenizerModel& model);
    static void parse_normalizer(const nlohmann::json& json_normalizer, UnicodeNormalizer& normalizer);
    static void parse_pre_tokenizer(const nlohmann::json& json_pre_tokenizer, std::shared_ptr<PreTokenizer>& pre_tokenizer, TokenizerConfig& config);
    static void parse_post_processor(const nlohmann::json& json_post_processor, std::shared_ptr<PostProcessor>& post_processor);
    static void parse_added_tokens(const nlohmann::json& json_added_tokens, TokenizerConfig& config);
};

} // namespace auratokenizer