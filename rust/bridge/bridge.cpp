#include "bridge.h"
#include "aura-tokenizer/src/ffi.rs.h"
#include "bpe_tokenizer.h"
#include "tokenizer_advanced.h"
#include "tokenizer_config.h"
#include "vocab.h"
#include "unicode_normalizer.h"
#include "pre_tokenizer.h"
#include "post_processor.h"
#include "tokenizer_model.h"

namespace auratokenizer {

TokenizerConfig from_rust_config(const RustTokenizerConfig& r_config) {
    TokenizerConfig config;
    config.unk_token = std::string(r_config.unk_token);
    config.pad_token = std::string(r_config.pad_token);
    config.bos_token = std::string(r_config.bos_token);
    config.eos_token = std::string(r_config.eos_token);
    config.mask_token = std::string(r_config.mask_token);
    config.sep_token = std::string(r_config.sep_token);
    config.cls_token = std::string(r_config.cls_token);
    return config;
}

std::unique_ptr<TokenizerAdvanced> create_bpe_tokenizer(const RustTokenizerConfig& r_config, const rust::String& vocab_path, const rust::String& merges_path) {
    auto config = from_rust_config(r_config);
    
    // TODO: Load actual BPE model if paths are provided
    auto model = std::make_shared<TokenizerModel>(); 
    model->set_algorithm(TokenizationAlgorithm::BPE);
    auto normalizer = std::make_shared<UnicodeNormalizer>(config);
    auto pre_tokenizer = std::make_shared<RegexPreTokenizer>(config);
    std::shared_ptr<PostProcessor> post_processor = nullptr;
    
    return std::make_unique<TokenizerAdvanced>(config, model, normalizer, pre_tokenizer, post_processor);
}

std::unique_ptr<TokenizerAdvanced> create_wordpiece_tokenizer(const RustTokenizerConfig& r_config, const rust::String& vocab_path) {
    auto config = from_rust_config(r_config);
    config.model_type = "wordpiece";
    config.base_model = ModelType::BERT;
    
    auto model = std::make_shared<TokenizerModel>();
    model->set_algorithm(TokenizationAlgorithm::WORDPIECE);
    
    auto normalizer = std::make_shared<UnicodeNormalizer>(config);
    auto pre_tokenizer = std::make_shared<RegexPreTokenizer>(config);
    std::shared_ptr<PostProcessor> post_processor = nullptr;
    
    auto tokenizer = std::make_unique<TokenizerAdvanced>(config, model, normalizer, pre_tokenizer, post_processor);
    
    // TODO: Load vocab from vocab_path if provided
    if (!vocab_path.empty()) {
        // tokenizer->load_vocab(std::string(vocab_path));
    }
    
    return tokenizer;
}

std::unique_ptr<TokenizerAdvanced> create_unigram_tokenizer(const RustTokenizerConfig& r_config, const rust::String& vocab_path) {
    auto config = from_rust_config(r_config);
    
    auto model = std::make_shared<TokenizerModel>();
    model->set_algorithm(TokenizationAlgorithm::UNIGRAM);
    
    auto normalizer = std::make_shared<UnicodeNormalizer>(config);
    auto pre_tokenizer = std::make_shared<RegexPreTokenizer>(config);
    std::shared_ptr<PostProcessor> post_processor = nullptr;
    
    auto tokenizer = std::make_unique<TokenizerAdvanced>(config, model, normalizer, pre_tokenizer, post_processor);
    
    // TODO: Load vocab and scores from vocab_path if provided
    if (!vocab_path.empty()) {
        // tokenizer->load_vocab(std::string(vocab_path));
    }
    
    return tokenizer;
}

std::unique_ptr<TokenizerAdvanced> create_charlevel_tokenizer(const RustTokenizerConfig& r_config) {
    auto config = from_rust_config(r_config);
    
    auto model = std::make_shared<TokenizerModel>();
    model->set_algorithm(TokenizationAlgorithm::CHARLEVEL);
    
    auto normalizer = std::make_shared<UnicodeNormalizer>(config);
    auto pre_tokenizer = std::make_shared<RegexPreTokenizer>(config);
    std::shared_ptr<PostProcessor> post_processor = nullptr;
    
    return std::make_unique<TokenizerAdvanced>(config, model, normalizer, pre_tokenizer, post_processor);
}

std::unique_ptr<TokenizerAdvanced> load_tokenizer(const rust::String& path) {
    TokenizerConfig config;
    auto model = std::make_shared<TokenizerModel>();
    auto normalizer = std::make_shared<UnicodeNormalizer>(config);
    auto pre_tokenizer = std::make_shared<RegexPreTokenizer>(config);
    std::shared_ptr<PostProcessor> post_processor = nullptr;
    
    auto tokenizer = std::make_unique<TokenizerAdvanced>(config, model, normalizer, pre_tokenizer, post_processor);
    tokenizer->load(std::string(path));
    return tokenizer;
}

rust::Vec<int> get_encoding_ids(const BatchEncoding& encoding, size_t index) {
    rust::Vec<int> result;
    if (index < encoding.input_ids.size()) {
        const auto& ids = encoding.input_ids[index];
        for (int id : ids) {
            result.push_back(id);
        }
    }
    return result;
}

rust::Vec<int> get_encoding_mask(const BatchEncoding& encoding, size_t index) {
    rust::Vec<int> result;
    if (index < encoding.attention_mask.size()) {
        const auto& mask = encoding.attention_mask[index];
        for (int m : mask) {
            result.push_back(m);
        }
    }
    return result;
}

void add_special_tokens(TokenizerAdvanced& tokenizer, const rust::Vec<rust::String>& tokens) {
    std::vector<std::string> cpp_tokens;
    for (const auto& t : tokens) {
        cpp_tokens.push_back(std::string(t));
    }
    tokenizer.add_special_tokens(cpp_tokens);
}

void save(const TokenizerAdvanced& tokenizer, const rust::String& path) {
    tokenizer.save(std::string(path));
}

std::unique_ptr<BatchEncoding> encode(const TokenizerAdvanced& tokenizer, const rust::String& text) {
    auto encoding = tokenizer.encode(std::string(text));
    return std::make_unique<BatchEncoding>(std::move(encoding));
}

rust::String decode(const TokenizerAdvanced& tokenizer, const rust::Vec<int>& ids, bool skip_special_tokens) {
    std::vector<int> cpp_ids;
    for (int id : ids) {
        cpp_ids.push_back(id);
    }
    return rust::String(tokenizer.decode(cpp_ids, skip_special_tokens));
}

int token_to_id(const TokenizerAdvanced& tokenizer, const rust::String& token) {
    return tokenizer.token_to_id(std::string(token));
}

rust::String id_to_token(const TokenizerAdvanced& tokenizer, int id) {
    return rust::String(tokenizer.id_to_token(id));
}

void train_from_files(TokenizerAdvanced& tokenizer, const rust::Vec<rust::String>& files, size_t vocab_size) {
    std::vector<std::string> cpp_files;
    for (const auto& file : files) {
        cpp_files.push_back(std::string(file));
    }
    tokenizer.train(cpp_files, vocab_size);
}

void set_normalization_form(TokenizerAdvanced& tokenizer, const rust::String& form_str) {
    std::string form = std::string(form_str);
    NormalizationForm f = NormalizationForm::NONE;
    if (form == "NFC") f = NormalizationForm::NFC;
    else if (form == "NFD") f = NormalizationForm::NFD;
    else if (form == "NFKC") f = NormalizationForm::NFKC;
    else if (form == "NFKD") f = NormalizationForm::NFKD;
    tokenizer.set_normalization_form(f);
}

void set_strip_accents(TokenizerAdvanced& tokenizer, bool strip) {
    tokenizer.set_strip_accents(strip);
}

void set_lowercase(TokenizerAdvanced& tokenizer, bool lowercase) {
    tokenizer.set_lowercase(lowercase);
}

void add_pre_tokenizer_pattern(TokenizerAdvanced& tokenizer, const rust::String& pattern) {
    tokenizer.add_pre_tokenizer_pattern(std::string(pattern));
}

void create_bert_post_processor(TokenizerAdvanced& tokenizer, bool add_special_tokens) {
    tokenizer.create_bert_post_processor(add_special_tokens);
}

} // namespace auratokenizer
