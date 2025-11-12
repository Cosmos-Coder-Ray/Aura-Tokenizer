#include "post_processor.h"
#include "template_parser.h"
#include "tokenizer_exception.h"

namespace auratokenizer {

    BertPostProcessor::BertPostProcessor(const TokenizerConfig& config, const Vocab& vocab)
        : cls_token_id_(vocab.get_token_id(config.cls_token)),
        cls_token_text_(config.cls_token),
        sep_token_id_(vocab.get_token_id(config.sep_token)),
        sep_token_text_(config.sep_token)
    {
    }

    std::vector<Token> BertPostProcessor::process(const std::vector<Token>& tokens) const {
        std::vector<Token> result;
        result.reserve(tokens.size() + 2);

        if (cls_token_id_ >= 0) result.emplace_back(cls_token_id_, cls_token_text_, true);
        result.insert(result.end(), tokens.begin(), tokens.end());
        if (sep_token_id_ >= 0) result.emplace_back(sep_token_id_, sep_token_text_, true);

        return result;
    }

    std::vector<int> BertPostProcessor::process_ids(const std::vector<int>& ids) const {
        std::vector<int> result;
        result.reserve(ids.size() + 2);

        if (cls_token_id_ >= 0) result.push_back(cls_token_id_);
        result.insert(result.end(), ids.begin(), ids.end());
        if (sep_token_id_ >= 0) result.push_back(sep_token_id_);

        return result;
    }

    TemplatePostProcessor::TemplatePostProcessor(const Template& templ, const Vocab& vocab)
        : template_(templ)
    {
        for (const auto& p_str : templ.prefix) {
            int id = vocab.get_token_id(p_str);
            if (id >= 0) {
                prefix_ids_.push_back(id);
                prefix_tokens_.emplace_back(id, p_str, vocab.is_special_token(p_str));
            }
        }
        for (const auto& s_str : templ.suffix) {
            int id = vocab.get_token_id(s_str);
            if (id >= 0) {
                suffix_ids_.push_back(id);
                suffix_tokens_.emplace_back(id, s_str, vocab.is_special_token(s_str));
            }
        }
    }

    std::vector<Token> TemplatePostProcessor::process(const std::vector<Token>& tokens) const {
        std::vector<Token> result;
        result.reserve(prefix_tokens_.size() + tokens.size() + suffix_tokens_.size());

        result.insert(result.end(), prefix_tokens_.begin(), prefix_tokens_.end());
        result.insert(result.end(), tokens.begin(), tokens.end());
        result.insert(result.end(), suffix_tokens_.begin(), suffix_tokens_.end());

        return result;
    }

    std::vector<int> TemplatePostProcessor::process_ids(const std::vector<int>& ids) const {
        std::vector<int> result;
        result.reserve(prefix_ids_.size() + ids.size() + suffix_ids_.size());

        result.insert(result.end(), prefix_ids_.begin(), prefix_ids_.end());
        result.insert(result.end(), ids.begin(), ids.end());
        result.insert(result.end(), suffix_ids_.begin(), suffix_ids_.end());

        return result;
    }

    ChatTemplatePostProcessor::ChatTemplatePostProcessor(const std::string& chat_template, const Vocab& vocab, const TokenizerConfig& config)
        : chat_template_(chat_template),
          vocab_(vocab),
          config_(config)
    {
        parsed_template_ = templates::TemplateParser::parse(chat_template_);
    }

    std::vector<Token> ChatTemplatePostProcessor::process(const std::vector<Token>& tokens) const {
        std::vector<Token> result;
        for (const auto& segment : parsed_template_) {
            if (segment.type == templates::TemplateSegment::LITERAL) {
                // For literals, we need to tokenize them. This assumes a basic tokenization
                // for the literal parts of the template. A more robust solution might
                // involve passing a pre-tokenizer here.
                // For simplicity, we'll just add them as single tokens for now.
                // In a real scenario, you'd likely want to tokenize these literals
                // using the main tokenizer's pre-tokenizer.
                for (char c : segment.value) {
                    std::string s(1, c);
                    result.emplace_back(vocab_.get_token_id(s), s, false);
                }
            } else if (segment.type == templates::TemplateSegment::VARIABLE) {
                if (segment.value == "message") {
                    result.insert(result.end(), tokens.begin(), tokens.end());
                } else {
                    // Handle other variables if needed, e.g., by looking them up in a context map
                    throw TokenizerException("Unsupported template variable: " + segment.value);
                }
            } else if (segment.type == templates::TemplateSegment::SPECIAL_TOKEN) {
                int id = resolve_special_token_id(segment.value);
                if (id != -1) {
                    result.emplace_back(id, vocab_.get_token_text(id), true);
                } else {
                    throw TokenizerException("Unknown special token in template: " + segment.value);
                }
            }
        }
        return result;
    }

    std::vector<int> ChatTemplatePostProcessor::process_ids(const std::vector<int>& ids) const {
        std::vector<int> result;
        for (const auto& segment : parsed_template_) {
            if (segment.type == templates::TemplateSegment::LITERAL) {
                // Similar to process, tokenize literals and add their IDs
                for (char c : segment.value) {
                    std::string s(1, c);
                    result.push_back(vocab_.get_token_id(s));
                }
            } else if (segment.type == templates::TemplateSegment::VARIABLE) {
                if (segment.value == "message") {
                    result.insert(result.end(), ids.begin(), ids.end());
                } else {
                    throw TokenizerException("Unsupported template variable: " + segment.value);
                }
            } else if (segment.type == templates::TemplateSegment::SPECIAL_TOKEN) {
                int id = resolve_special_token_id(segment.value);
                if (id != -1) {
                    result.push_back(id);
                } else {
                    throw TokenizerException("Unknown special token in template: " + segment.value);
                }
            }
        }
        return result;
    }

    int ChatTemplatePostProcessor::resolve_special_token_id(const std::string& token_name) const {
        // This mapping needs to be robust. It should map the template's special token
        // name (e.g., "CLS") to the actual special token string used in the vocabulary
        // (e.g., config_.cls_token).
        if (token_name == "CLS") return vocab_.get_token_id(config_.cls_token);
        if (token_name == "SEP") return vocab_.get_token_id(config_.sep_token);
        if (token_name == "BOS") return vocab_.get_token_id(config_.bos_token);
        if (token_name == "EOS") return vocab_.get_token_id(config_.eos_token);
        if (token_name == "PAD") return vocab_.get_token_id(config_.pad_token);
        if (token_name == "UNK") return vocab_.get_token_id(config_.unk_token);
        if (token_name == "MASK") return vocab_.get_token_id(config_.mask_token);

        // Fallback for custom special tokens defined in config.added_tokens
        for (const auto& pair : config_.added_tokens) {
            if (pair.first == token_name) {
                return vocab_.get_token_id(pair.first);
            }
        }

        return -1; // Not found
    }

    void CompositePostProcessor::add_processor(std::shared_ptr<PostProcessor> processor) {
        if (processor) {
            processors_.push_back(std::move(processor));
        }
    }

    std::vector<Token> CompositePostProcessor::process(const std::vector<Token>& tokens) const {
        std::vector<Token> output = tokens;
        for (const auto& processor : processors_) {
            output = processor->process(output);
        }
        return output;
    }

    std::vector<int> CompositePostProcessor::process_ids(const std::vector<int>& ids) const {
        std::vector<int> output = ids;
        for (const auto& processor : processors_) {
            output = processor->process_ids(output);
        }
        return output;
    }

    std::shared_ptr<PostProcessor> create_post_processor(const TokenizerConfig& config, const Vocab& vocab) {
        auto composite = std::make_shared<CompositePostProcessor>();

        if (config.base_model == ModelType::BERT) {
            composite->add_processor(std::make_shared<BertPostProcessor>(config, vocab));
        }

        // Add ChatTemplatePostProcessor if a chat template is provided
        if (!config.chat_template.empty()) {
            composite->add_processor(std::make_shared<ChatTemplatePostProcessor>(config.chat_template, vocab, config));
        }

        // You can add more logic here for other model types or template styles.

        return composite;
    }

}