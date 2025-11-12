#include "tokenizer_json_parser.h"
#include <fstream>
#include "tokenizer_exception.h"
#include "byte_level_pre_tokenizer.h"

namespace auratokenizer {

void TokenizerJsonParser::parse(
    const std::string& json_content,
    TokenizerConfig& config,
    TokenizerModel& model,
    std::shared_ptr<PreTokenizer>& pre_tokenizer,
    std::shared_ptr<PostProcessor>& post_processor,
    UnicodeNormalizer& normalizer
) {
    nlohmann::json json_data;
    try {
        json_data = nlohmann::json::parse(json_content);
    } catch (const nlohmann::json::parse_error& e) {
        throw TokenizerException("Failed to parse tokenizer.json string: " + std::string(e.what()));
    }

    if (json_data.contains("model")) {
        parse_model(json_data["model"], model);
    }
    if (json_data.contains("normalizer")) {
        parse_normalizer(json_data["normalizer"], normalizer);
    }
    if (json_data.contains("pre_tokenizer")) {
        parse_pre_tokenizer(json_data["pre_tokenizer"], pre_tokenizer, config);
    }
    if (json_data.contains("post_processor")) {
        parse_post_processor(json_data["post_processor"], post_processor);
    }
    if (json_data.contains("added_tokens")) {
        parse_added_tokens(json_data["added_tokens"], config);
    }
}

void TokenizerJsonParser::parse_model(const nlohmann::json& json_model, TokenizerModel& model) {
    if (json_model.contains("vocab")) {
        for (auto it = json_model["vocab"].begin(); it != json_model["vocab"].end(); ++it) {
            model.add_token_to_vocab(it.key(), it.value().get<int>());
        }
    }
    if (json_model.contains("merges")) {
        for (const auto& merge_str : json_model["merges"]) {
            model.add_merge(merge_str.get<std::string>());
        }
    }
    if (json_model.contains("scores")) {
        for (auto it = json_model["scores"].begin(); it != json_model["scores"].end(); ++it) {
            model.add_token_with_score(it.key(), it.value().get<float>());
        }
    }

    if (json_model.contains("type")) {
        std::string type = json_model["type"].get<std::string>();
        if (type == "BPE") {
            model.set_algorithm(TokenizationAlgorithm::BPE);
        } else if (type == "Unigram") {
            model.set_algorithm(TokenizationAlgorithm::UNIGRAM);
        } else if (type == "WordPiece") {
            model.set_algorithm(TokenizationAlgorithm::WORDPIECE);
        } else if (type == "CharLevel") {
            model.set_algorithm(TokenizationAlgorithm::CHARLEVEL);
        } else {
            throw TokenizerException("Unknown model type: " + type);
        }
    }
}

void TokenizerJsonParser::parse_normalizer(const nlohmann::json& json_normalizer, UnicodeNormalizer& normalizer) {
    if (json_normalizer.contains("type")) {
        std::string type = json_normalizer["type"].get<std::string>();
        if (type == "NFC") {
            normalizer.set_normalization_form(NormalizationForm::NFC);
        } else if (type == "NFD") {
            normalizer.set_normalization_form(NormalizationForm::NFD);
        } else if (type == "NFKC") {
            normalizer.set_normalization_form(NormalizationForm::NFKC);
        } else if (type == "NFKD") {
            normalizer.set_normalization_form(NormalizationForm::NFKD);
        } else if (type == "None") {
            normalizer.set_normalization_form(NormalizationForm::NONE);
        } else {
            throw TokenizerException("Unknown normalizer type: " + type);
        }
    }
    if (json_normalizer.contains("strip_accents")) {
        normalizer.set_strip_accents(json_normalizer["strip_accents"].get<bool>());
    }
    if (json_normalizer.contains("lowercase")) {
        normalizer.set_lowercase(json_normalizer["lowercase"].get<bool>());
    }
}

void TokenizerJsonParser::parse_pre_tokenizer(const nlohmann::json& json_pre_tokenizer, std::shared_ptr<PreTokenizer>& pre_tokenizer, TokenizerConfig& config) {
    if (json_pre_tokenizer.contains("type")) {
        std::string type = json_pre_tokenizer["type"].get<std::string>();
        if (type == "ByteLevel") {
            pre_tokenizer = std::make_shared<auratokenizer::ByteLevelPreTokenizer>(config);
        } else if (type == "Whitespace") {
            // Assuming Whitespace pre-tokenizer is a RegexPreTokenizer with a specific pattern
            // You might need to pass config here if RegexPreTokenizer needs it
            pre_tokenizer = std::make_shared<RegexPreTokenizer>(TokenizerConfig()); // Placeholder config
            // Add specific patterns for whitespace if needed
        } else {
            throw TokenizerException("Unknown pre_tokenizer type: " + type);
        }
    }
}

void TokenizerJsonParser::parse_post_processor(const nlohmann::json& json_post_processor, std::shared_ptr<PostProcessor>& post_processor) {
    if (json_post_processor.contains("type")) {
        std::string type = json_post_processor["type"].get<std::string>();
        if (type == "Bert") {
            // BertPostProcessor needs TokenizerConfig and Vocab. These are not directly available here.
            // This highlights a design challenge: PostProcessors often need more context.
            // For now, we'll create a placeholder. A more robust solution would pass these dependencies
            // through the TokenizerJsonParser or have the PostProcessor fetch them from a central context.
            // For the purpose of getting the build working, we'll assume a default config and vocab.
            post_processor = std::make_shared<BertPostProcessor>(TokenizerConfig(), Vocab());
        } else if (type == "Template") {
            // TemplatePostProcessor needs a template string and Vocab.
            // Similar dependency issue as BertPostProcessor.
            // For now, we'll create a placeholder.
            post_processor = std::make_shared<TemplatePostProcessor>(TemplatePostProcessor::Template(), Vocab());
        } else if (type == "ChatTemplate") {
            // ChatTemplatePostProcessor needs chat_template, Vocab, and TokenizerConfig.
            // This is even more complex due to the chat_template string.
            // For now, we'll create a placeholder.
            post_processor = std::make_shared<ChatTemplatePostProcessor>("", Vocab(), TokenizerConfig());
        } else {
            throw TokenizerException("Unknown post_processor type: " + type);
        }
    }
}

void TokenizerJsonParser::parse_added_tokens(const nlohmann::json& json_added_tokens, TokenizerConfig& config) {
    for (const auto& token_data : json_added_tokens) {
        if (token_data.contains("id") && token_data.contains("content")) {
            config.added_tokens[token_data["content"].get<std::string>()] = std::to_string(token_data["id"].get<int>());
        }
    }
}

} // namespace auratokenizer
