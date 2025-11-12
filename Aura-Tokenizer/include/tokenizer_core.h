#pragma once

#include "offsets.h"
#include "token.h"
#include "tokenizer_config.h"
#include "tokenizer_exception.h"
#include "plugin_registry.h"

#include <memory>
#include <string>
#include <vector>

namespace auratokenizer {

    class Vocab; // Forward declaration

    // Forward declaration for PluginRegistry
    class PluginRegistry;

    class TokenizerBase {
    public:
        explicit TokenizerBase(const TokenizerConfig& config) : config_(config) {}
        virtual ~TokenizerBase() = default;

        virtual std::vector<Token> encode(const std::string& text) = 0;
        virtual std::vector<int> encode_to_ids(const std::string& text) = 0;
        virtual std::string decode(const std::vector<Token>& tokens) = 0;
        virtual std::string decode_from_ids(const std::vector<int>& ids) = 0;
        virtual std::vector<std::vector<int>> batch_encode(const std::vector<std::string>& texts) = 0;
        virtual std::vector<std::string> batch_decode(const std::vector<std::vector<int>>& ids) = 0;

        virtual void train(const std::vector<std::string>& texts, size_t vocab_size) = 0;
        virtual void save(const std::string& path) = 0;
        virtual void load(const std::string& path) = 0;

        virtual void add_special_tokens(const std::vector<std::string>& tokens) = 0;
        virtual std::vector<std::string> get_special_tokens() const = 0;
        virtual bool is_special_token(const std::string& token) const = 0;

        virtual const TokenizerConfig& get_config() const = 0;
        virtual void set_config(const TokenizerConfig& config) = 0;
        virtual void set_vocab(std::shared_ptr<Vocab> vocab) = 0;

    protected:
        TokenizerConfig config_;

        // --- Streaming Support ---
        /**
         * @brief Tokenize a chunk of text in a streaming fashion (optional override).
         * @param text_chunk The input text chunk.
         * @return A vector of token IDs.
         */
        virtual std::vector<int> tokenize_chunk(const std::string& text_chunk) { return encode_to_ids(text_chunk); }

        /**
         * @brief Feed a chunk of text for incremental training (optional override).
         * @param text_chunk The input text chunk.
         */
        virtual void train_on_chunk(const std::string& text_chunk) { /* Optional: override in derived classes */ }

        /**
         * @brief Finalize streaming training (optional override).
         */
        virtual void finalize_training() { /* Optional: override in derived classes */ }

        // --- Offset Tracking ---
        /**
         * @brief Compute token offsets for a given text and its tokenized output (optional override).
         * @param text The original input text.
         * @param tokens The tokenized output (as strings).
         * @return A vector of TokenOffset structs.
         */
        virtual std::vector<TokenOffset> compute_offsets(const std::string& text, const std::vector<std::string>& tokens) { return {}; }

        // --- Plugin Support ---
        /**
         * @brief Register a custom plugin/component (optional override).
         * @param name The unique name of the component.
         * @param factory The factory function to create the component.
         */
        virtual void register_plugin(const std::string& name, PluginRegistry::FactoryFunc factory) { PluginRegistry::instance().register_plugin(name, factory); }
    };

} // namespace auratokenizer