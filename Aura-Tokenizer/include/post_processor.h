#pragma once

#include "tokenizer_core.h"
#include "vocab.h" // For Vocab
#include "template_parser.h" // For templates::ParsedTemplate
#include <memory>
#include <vector>
#include <string>

namespace auratokenizer {

    class PostProcessor {
    public:
        virtual ~PostProcessor() = default;
        virtual std::vector<Token> process(const std::vector<Token>& tokens) const = 0;
        virtual std::vector<int> process_ids(const std::vector<int>& ids) const = 0;
    };

    class BertPostProcessor : public PostProcessor {
    public:
        explicit BertPostProcessor(const TokenizerConfig& config, const Vocab& vocab);
        std::vector<Token> process(const std::vector<Token>& tokens) const override;
        std::vector<int> process_ids(const std::vector<int>& ids) const override;

    private:
        int cls_token_id_;
        std::string cls_token_text_;
        int sep_token_id_;
        std::string sep_token_text_;
    };

    class TemplatePostProcessor : public PostProcessor {
    public:
        struct Template {
            std::vector<std::string> prefix;
            std::vector<std::string> suffix;
        };

        TemplatePostProcessor(const Template& templ, const Vocab& vocab);
        std::vector<Token> process(const std::vector<Token>& tokens) const override;
        std::vector<int> process_ids(const std::vector<int>& ids) const override;

    private:
        Template template_;
        std::vector<Token> prefix_tokens_;
        std::vector<int> prefix_ids_;
        std::vector<Token> suffix_tokens_;
        std::vector<int> suffix_ids_;
    };

    class ChatTemplatePostProcessor : public PostProcessor {
    public:
        ChatTemplatePostProcessor(const std::string& chat_template, const Vocab& vocab, const TokenizerConfig& config);
        std::vector<Token> process(const std::vector<Token>& tokens) const override;
        std::vector<int> process_ids(const std::vector<int>& ids) const override;

    private:
        std::string chat_template_;
        const Vocab& vocab_;
        const TokenizerConfig& config_;
        templates::ParsedTemplate parsed_template_;

        // Helper to resolve special tokens from the template
        int resolve_special_token_id(const std::string& token_name) const;
    };

    class CompositePostProcessor : public PostProcessor {
    public:
        void add_processor(std::shared_ptr<PostProcessor> processor);
        std::vector<Token> process(const std::vector<Token>& tokens) const override;
        std::vector<int> process_ids(const std::vector<int>& ids) const override;

    private:
        std::vector<std::shared_ptr<PostProcessor>> processors_;
    };

    std::shared_ptr<PostProcessor> create_post_processor(const TokenizerConfig& config, const Vocab& vocab);

}