#include "tokenizer_advanced.h"
#include "serialization_utils.h"
#include "bpe_tokenizer.h"
#include "unigram_tokenizer.h"
#include "char_level_tokenizer.h"
#include "wordpiece_tokenizer.h"
#include "wordpiece_model.h"
#include "bpe_trainer.h"
#include "unigram_trainer.h"

#include <fstream>
#include <numeric>

namespace auratokenizer {

    TokenizerAdvanced::TokenizerAdvanced(
        const TokenizerConfig& config,
        std::shared_ptr<TokenizerModel> model,
        std::shared_ptr<UnicodeNormalizer> normalizer,
        std::shared_ptr<auratokenizer::PreTokenizer> pre_tokenizer,
        std::shared_ptr<PostProcessor> post_processor
    )
        : config_(config),
        model_(model),
        normalizer_(normalizer),
        pre_tokenizer_(pre_tokenizer),
        post_processor_(post_processor),
        vocab_(std::make_shared<Vocab>()), // Vocab is still managed internally
        special_tokens_map_(),
        id_to_special_token_()
    {
        // Init with config special tokens
        if (!config_.unk_token.empty()) {
            vocab_->add_special_token(config_.unk_token, SpecialTokenType::UNK);
            special_tokens_map_[config_.unk_token] = vocab_->get_token_id(config_.unk_token);
            id_to_special_token_[vocab_->get_token_id(config_.unk_token)] = config_.unk_token;
        }
        if (!config_.pad_token.empty()) {
            vocab_->add_special_token(config_.pad_token, SpecialTokenType::PAD);
            special_tokens_map_[config_.pad_token] = vocab_->get_token_id(config_.pad_token);
            id_to_special_token_[vocab_->get_token_id(config_.pad_token)] = config_.pad_token;
        }
        if (!config_.bos_token.empty()) {
            vocab_->add_special_token(config_.bos_token, SpecialTokenType::BOS);
            special_tokens_map_[config_.bos_token] = vocab_->get_token_id(config_.bos_token);
            id_to_special_token_[vocab_->get_token_id(config_.bos_token)] = config_.bos_token;
        }
        if (!config_.eos_token.empty()) {
            vocab_->add_special_token(config_.eos_token, SpecialTokenType::EOS);
            special_tokens_map_[config_.eos_token] = vocab_->get_token_id(config_.eos_token);
            id_to_special_token_[vocab_->get_token_id(config_.eos_token)] = config_.eos_token;
        }
        if (!config_.mask_token.empty()) {
            vocab_->add_special_token(config_.mask_token, SpecialTokenType::MASK);
            special_tokens_map_[config_.mask_token] = vocab_->get_token_id(config_.mask_token);
            id_to_special_token_[vocab_->get_token_id(config_.mask_token)] = config_.mask_token;
        }
        if (!config_.sep_token.empty()) {
            vocab_->add_special_token(config_.sep_token, SpecialTokenType::SEP);
            special_tokens_map_[config_.sep_token] = vocab_->get_token_id(config_.sep_token);
            id_to_special_token_[vocab_->get_token_id(config_.sep_token)] = config_.sep_token;
        }
        if (!config_.cls_token.empty()) {
            vocab_->add_special_token(config_.cls_token, SpecialTokenType::CLS);
            special_tokens_map_[config_.cls_token] = vocab_->get_token_id(config_.cls_token);
            id_to_special_token_[vocab_->get_token_id(config_.cls_token)] = config_.cls_token;
        }

        // Populate vocab from model
        if (model_) {
            for (const std::pair<const std::string, int>& pair_vocab : model_->get_vocab()) {
                vocab_->add_token_to_vocab(pair_vocab.first, pair_vocab.second);
            }
            for (const std::pair<const std::string, float>& pair_scores : model_->get_scores()) {
                vocab_->add_token_with_score(pair_scores.first, pair_scores.second);
            }
        }

        // Populate vocab with added tokens from config
        for (const std::pair<const std::string, std::string>& pair_added : config_.added_tokens) {
            vocab_->add_special_token(pair_added.first, SpecialTokenType::CUSTOM);
            special_tokens_map_[pair_added.first] = vocab_->get_token_id(pair_added.first);
            id_to_special_token_[vocab_->get_token_id(pair_added.first)] = pair_added.first;
        }

        // Initialize the concrete tokenizer implementation based on the model algorithm
        std::shared_ptr<BPETokenizer> bpe_tokenizer;
        std::shared_ptr<UnigramTokenizer> unigram_tokenizer;
        std::shared_ptr<WordPieceTokenizer> wordpiece_tokenizer;
        std::shared_ptr<CharLevelTokenizer> char_level_tokenizer;

        switch (static_cast<int>(model_->get_algorithm())) {
            case static_cast<int>(TokenizationAlgorithm::BPE):
                bpe_tokenizer = std::make_shared<BPETokenizer>(config_);
                bpe_tokenizer->set_merge_rules(model_->get_merges());
                tokenizer_impl_ = bpe_tokenizer;
                break;
            case static_cast<int>(TokenizationAlgorithm::UNIGRAM):
                unigram_tokenizer = std::make_shared<UnigramTokenizer>(config_);
                unigram_tokenizer->set_vocab_and_scores(vocab_, model_->get_scores());
                tokenizer_impl_ = unigram_tokenizer;
                break;
            case static_cast<int>(TokenizationAlgorithm::WORDPIECE):
                {
                    wordpiece_tokenizer = std::make_shared<WordPieceTokenizer>(config_);
                    auto wordpiece_model = std::make_shared<models::WordPieceModel>();
                    wordpiece_model->initialize(model_->get_vocab(), config_.unk_token, 100); // Assuming unk_token is in config
                    wordpiece_tokenizer->set_wordpiece_model(wordpiece_model);
                    tokenizer_impl_ = wordpiece_tokenizer;
                }
                break;
            case static_cast<int>(TokenizationAlgorithm::CHARLEVEL):
                char_level_tokenizer = std::make_shared<CharLevelTokenizer>(config_);
                tokenizer_impl_ = char_level_tokenizer;
                break;
            default:
                throw TokenizerException("Unsupported tokenization algorithm.");
        }

        // Set the vocab for the tokenizer implementation
        if (tokenizer_impl_) {
            tokenizer_impl_->set_vocab(vocab_);
        }
    }

    BatchEncoding TokenizerAdvanced::encode(const std::string& text, const TokenizationOptions& options) const {
        if (!tokenizer_impl_) {
            throw TokenizerException("Tokenizer not initialized.");
        }

        std::string normalized = normalizer_->normalize(text);
        std::vector<std::string> pre_tokenized;
        if (pre_tokenizer_) {
            pre_tokenized = pre_tokenizer_->pre_tokenize(normalized);
        } else {
            // Default pre-tokenization if none is provided
            pre_tokenized = {normalized};
        }

        std::vector<int> ids;
        // This part needs to be adapted to handle pre_tokenized input and then
        // delegate to the specific tokenizer_impl_ (BPE, Unigram, CharLevel).
        // For now, a simplified delegation:
        if (pre_tokenized.size() == 1) {
            ids = tokenizer_impl_->encode_to_ids(pre_tokenized[0]);
        } else {
            // Handle multiple pre-tokens (e.g., for WordPiece or complex BPE)
            // This will require more sophisticated logic, potentially iterating
            // through pre_tokenized and encoding each part.
            // For now, just concatenate for simplicity.
            std::string combined_text;
            for(const auto& t : pre_tokenized) {
                combined_text += t;
            }
            ids = tokenizer_impl_->encode_to_ids(combined_text);
        }

        // Apply post-processing if available
        if (post_processor_) {
            ids = post_processor_->process_ids(ids);
        }

        BatchEncoding result;
        result.input_ids.push_back(ids);
        if (options.return_attention_mask) {
            // simplified mask generation
            result.attention_mask.emplace_back(ids.size(), 1);
        }
        return result;
    }

    std::string TokenizerAdvanced::decode(const std::vector<int>& ids, bool skip_special_tokens) const {
        if (!tokenizer_impl_) {
            throw TokenizerException("Tokenizer not initialized.");
        }
        return tokenizer_impl_->decode_from_ids(ids);
    }

    // Other batch and advanced methods would follow a similar pattern
    BatchEncoding TokenizerAdvanced::encode_batch(const std::vector<std::string>& texts, const TokenizationOptions& options) const {
        BatchEncoding encoding;
        encoding.input_ids.reserve(texts.size());
        encoding.attention_mask.reserve(texts.size());
        encoding.token_type_ids.reserve(texts.size());
        encoding.offset_mapping.reserve(texts.size());
        encoding.length.reserve(texts.size());
        for (const auto& text : texts) {
            auto single_encoding = encode(text, options);
            if (!single_encoding.input_ids.empty()) {
                encoding.input_ids.push_back(single_encoding.input_ids[0]);
                if (options.return_attention_mask && !single_encoding.attention_mask.empty())
                    encoding.attention_mask.push_back(single_encoding.attention_mask[0]);
                if (options.return_token_type_ids && !single_encoding.token_type_ids.empty())
                    encoding.token_type_ids.push_back(single_encoding.token_type_ids[0]);
                if (!single_encoding.offset_mapping.empty())
                    encoding.offset_mapping.push_back(single_encoding.offset_mapping[0]);
                encoding.length.push_back(single_encoding.input_ids[0].size());
            }
        }
        return encoding;
    }

    std::vector<std::string> TokenizerAdvanced::decode_batch(const std::vector<std::vector<int>>& ids_batch, bool skip_special_tokens) const {
        if (!tokenizer_impl_) {
            throw TokenizerException("Tokenizer not initialized.");
        }
        std::vector<std::string> results;
        results.reserve(ids_batch.size());
        for (const auto& ids : ids_batch) {
            results.push_back(tokenizer_impl_->decode_from_ids(ids));
        }
        return results;
    }

    void TokenizerAdvanced::add_special_tokens(const std::vector<std::string>& tokens) {
        vocab_->add_tokens(tokens); // Add as special later if needed
    }

    void TokenizerAdvanced::save(const std::string& path) const {
        throw TokenizerException("TokenizerAdvanced::save not implemented yet.");
    }

    void TokenizerAdvanced::load(const std::string& path) {
        throw TokenizerException("TokenizerAdvanced::load not implemented yet.");
    }

    size_t TokenizerAdvanced::get_vocab_size() const {
        return vocab_->size();
    }

    int TokenizerAdvanced::token_to_id(const std::string& token) const {
        return vocab_->get_token_id(token);
    }

    std::string TokenizerAdvanced::id_to_token(int id) const {
        return vocab_->get_token(id);
    }

    void TokenizerAdvanced::train(const std::vector<std::string>& files, size_t vocab_size) {
        auto algo = model_->get_algorithm();
        if (algo == TokenizationAlgorithm::BPE) {
            BPETrainer trainer(config_);
            trainer.set_vocab_size(vocab_size);
            trainer.train_from_files(files, vocab_);
            
            // Update model merges
            // Note: BPETrainer returns pairs, model expects strings. We need to format them.
            // Assuming standard "token1 token2" format for now.
            for (const auto& rule : trainer.get_merge_rules()) {
                model_->add_merge(rule.first + " " + rule.second);
            }
        } else if (algo == TokenizationAlgorithm::UNIGRAM) {
            UnigramTrainer trainer(config_);
            // Unigram trainer usually determines vocab size automatically or via max_tokens
            // trainer.set_max_tokens(vocab_size); // Check if UnigramTrainer has this
            trainer.train_from_files(files);
            
            // UnigramTrainer creates a new vocab, so we update ours
            auto new_vocab = trainer.get_vocab();
            if (new_vocab) {
                // Copy entries to our vocab
                // This is a bit inefficient, better if we could swap the shared_ptr
                // but vocab_ is const in some contexts or shared.
                // For now, let's assume we can just replace the content or the pointer if possible.
                // Since vocab_ is shared_ptr<Vocab>, we can point it to the new one?
                // But other components might hold references to the old one.
                // Safer to clear and copy.
                vocab_->clear();
                // We need a way to iterate over new_vocab. Vocab class doesn't seem to expose iteration easily
                // except via internal maps which are private.
                // Actually, UnigramTrainer::get_vocab() returns a shared_ptr<Vocab>.
                // If we can replace vocab_, that's best.
                vocab_ = new_vocab;
                
                // Also update model scores
                // model_->set_scores(...) // TokenizerModel needs a setter for scores
            }
        } else {
            throw TokenizerException("Training is not supported for this algorithm.");
        }
        
        // Re-initialize implementation with new vocab
        if (tokenizer_impl_) {
            tokenizer_impl_->set_vocab(vocab_);
        }
    }

    void TokenizerAdvanced::set_normalization_form(NormalizationForm form) {
        if (normalizer_) normalizer_->set_normalization_form(form);
    }

    void TokenizerAdvanced::set_strip_accents(bool strip) {
        if (normalizer_) normalizer_->set_strip_accents(strip);
    }

    void TokenizerAdvanced::set_lowercase(bool lowercase) {
        if (normalizer_) normalizer_->set_lowercase(lowercase);
    }

    void TokenizerAdvanced::add_pre_tokenizer_pattern(const std::string& pattern) {
        auto regex_pre = std::dynamic_pointer_cast<RegexPreTokenizer>(pre_tokenizer_);
        if (regex_pre) {
            regex_pre->add_pattern(pattern);
        }
    }

    void TokenizerAdvanced::create_bert_post_processor(bool add_special_tokens) {
        if (vocab_) {
            // We might want to update config based on add_special_tokens if needed
            // For now, just create the processor
            post_processor_ = std::make_shared<BertPostProcessor>(config_, *vocab_);
        }
    }

}