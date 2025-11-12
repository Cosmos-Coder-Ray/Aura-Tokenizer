#include "tokenizer_wasm.h"
#include "vector_conversions.h"
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <memory>
#include <nlohmann/json.hpp>

namespace auratokenizer {

using namespace emscripten;
using json = nlohmann::json;

// Convert C++ vectors to JS arrays
template<typename T>
val vector_to_js_array(const std::vector<T>& vec) {
    val array = val::array();
    for (size_t i = 0; i < vec.size(); ++i) {
        array.set(i, vec[i]);
    }
    return array;
}

// Convert JS arrays to C++ vectors
template<typename T>
std::vector<T> js_array_to_vector(const val& arr) {
    std::vector<T> vec;
    const auto length = arr["length"].as<unsigned>();
    vec.reserve(length);
    for (unsigned i = 0; i < length; ++i) {
        vec.push_back(arr[i].as<T>());
    }
    return vec;
}

// WASM-specific TokenizerWASM implementation
TokenizerWASM::TokenizerWASM(const std::string& config_json) 
    : tokenizer_(std::make_shared<BPETokenizer>(config_json)) {}

val TokenizerWASM::encode(const std::string& text) {
    auto tokens = tokenizer_->encode(text);
    auto token_vec = to_token_vec(tokens);
    return vector_to_js_array(token_vec->tokens);
}

val TokenizerWASM::encode_to_ids(const std::string& text) {
    auto ids = tokenizer_->encode_to_ids(text);
    auto id_vec = to_id_vec(ids);
    return vector_to_js_array(id_vec->ids);
}

std::string TokenizerWASM::decode(const val& tokens) {
    auto cpp_tokens = js_array_to_vector<Token>(tokens);
    return tokenizer_->decode(cpp_tokens);
}

std::string TokenizerWASM::decode_from_ids(const val& ids) {
    auto cpp_ids = js_array_to_vector<int32_t>(ids);
    return tokenizer_->decode_from_ids(cpp_ids);
}

val TokenizerWASM::batch_encode(const val& texts) {
    auto cpp_texts = js_array_to_vector<std::string>(texts);
    auto str_vec = to_string_vec(cpp_texts);
    auto result = tokenizer_->batch_encode(*str_vec);
    auto id_vec_vec = to_id_vec_vec(result);
    return vector_to_js_array(id_vec_vec->data);
}

val TokenizerWASM::batch_decode(const val& ids) {
    auto cpp_ids = js_array_to_vector<std::vector<int32_t>>(ids);
    auto result = tokenizer_->batch_decode(cpp_ids);
    auto str_vec = to_string_vec(result);
    return vector_to_js_array(str_vec->strings);
}

void TokenizerWASM::train(const val& texts, uint32_t vocab_size) {
    auto cpp_texts = js_array_to_vector<std::string>(texts);
    auto str_vec = to_string_vec(cpp_texts);
    tokenizer_->train(*str_vec, vocab_size);
}

void TokenizerWASM::save(const std::string& path) {
    tokenizer_->save(path);
}

void TokenizerWASM::load(const std::string& path) {
    tokenizer_->load(path);
}

val TokenizerWASM::get_special_tokens() {
    auto tokens = tokenizer_->get_special_tokens();
    auto str_vec = to_string_vec(tokens);
    return vector_to_js_array(str_vec->strings);
}

bool TokenizerWASM::is_special_token(const std::string& token) {
    return tokenizer_->is_special_token(token);
}

val TokenizerWASM::get_config() {
    return val(tokenizer_->get_config()->to_json());
}

// Emscripten bindings
EMSCRIPTEN_BINDINGS(aura_tokenizer) {
    class_<TokenizerWASM>("Tokenizer")
        .constructor<std::string>()
        .function("encode", &TokenizerWASM::encode)
        .function("encodeToIds", &TokenizerWASM::encode_to_ids)
        .function("decode", &TokenizerWASM::decode)
        .function("decodeFromIds", &TokenizerWASM::decode_from_ids)
        .function("batchEncode", &TokenizerWASM::batch_encode)
        .function("batchDecode", &TokenizerWASM::batch_decode)
        .function("train", &TokenizerWASM::train)
        .function("save", &TokenizerWASM::save)
        .function("load", &TokenizerWASM::load)
        .function("getSpecialTokens", &TokenizerWASM::get_special_tokens)
        .function("isSpecialToken", &TokenizerWASM::is_special_token)
        .function("getConfig", &TokenizerWASM::get_config);

    value_object<Token>("Token")
        .field("id", &Token::id)
        .field("text", &Token::text)
        .field("start", &Token::start)
        .field("end", &Token::end);
}

} // namespace auratokenizer

EMSCRIPTEN_KEEPALIVE bool initialize_tokenizer(const char* config_json) {
    std::lock_guard<std::mutex> lock(wasm_mutex);
    try {
        TokenizerConfig config;
        TokenizerModel model;
        UnicodeNormalizer normalizer(config);
        std::shared_ptr<PreTokenizer> pre_tokenizer;
        std::shared_ptr<PostProcessor> post_processor;
        TokenizerJsonParser::parse(
            config_json,
            config,
            model,
            pre_tokenizer,
            post_processor,
            normalizer
        );
        tokenizer_instance = std::make_unique<TokenizerAdvanced>(
            config,
            std::make_shared<TokenizerModel>(model),
            std::make_shared<UnicodeNormalizer>(normalizer),
            pre_tokenizer,
            post_processor
        );
        return true;
    } catch (...) {
        return false;
    }
}

EMSCRIPTEN_KEEPALIVE char* encode(const char* text, const char* options_json) {
    std::lock_guard<std::mutex> lock(wasm_mutex);
    if (!tokenizer_instance) return nullptr;
    try {
        TokenizationOptions options;
        options.load_from_json_string(options_json);
        auto encoded = tokenizer_instance->encode(text, options);
        json result_json = encoded.input_ids;
        std::string result_str = result_json.dump();
        char* buffer = (char*)wasm_malloc(result_str.size() + 1);
        std::strcpy(buffer, result_str.c_str());
        return buffer;
    } catch (...) {
        return nullptr;
    }
}

EMSCRIPTEN_KEEPALIVE char* decode(const int* ids, int ids_length, bool skip_special_tokens) {
    std::lock_guard<std::mutex> lock(wasm_mutex);
    if (!tokenizer_instance) return nullptr;
    try {
        std::vector<int> id_vec(ids, ids + ids_length);
        std::string decoded = tokenizer_instance->decode(id_vec, skip_special_tokens);
        char* buffer = (char*)wasm_malloc(decoded.size() + 1);
        std::strcpy(buffer, decoded.c_str());
        return buffer;
    } catch (...) {
        return nullptr;
    }
}

EMSCRIPTEN_KEEPALIVE char* encode_batch(const char** texts, int texts_length, const char* options_json) {
    std::lock_guard<std::mutex> lock(wasm_mutex);
    if (!tokenizer_instance) return nullptr;
    try {
        TokenizationOptions options;
        options.load_from_json_string(options_json);
        std::vector<std::string> input;
        for (int i = 0; i < texts_length; ++i) {
            input.emplace_back(texts[i]);
        }
        auto encoded = tokenizer_instance->encode_batch(input, options);
        json result_json = encoded.input_ids;
        std::string result_str = result_json.dump();
        char* buffer = (char*)wasm_malloc(result_str.size() + 1);
        std::strcpy(buffer, result_str.c_str());
        return buffer;
    } catch (...) {
        return nullptr;
    }
}

EMSCRIPTEN_KEEPALIVE char* decode_batch(const int** ids_list, int* ids_lengths, int batch_size, bool skip_special_tokens) {
    std::lock_guard<std::mutex> lock(wasm_mutex);
    if (!tokenizer_instance) return nullptr;
    try {
        std::vector<std::vector<int>> all_ids;
        for (int i = 0; i < batch_size; ++i) {
            all_ids.emplace_back(ids_list[i], ids_list[i] + ids_lengths[i]);
        }
        auto decoded = tokenizer_instance->decode_batch(all_ids);
        json result_json = decoded;
        std::string result_str = result_json.dump();
        char* buffer = (char*)wasm_malloc(result_str.size() + 1);
        std::strcpy(buffer, result_str.c_str());
        return buffer;
    } catch (...) {
        return nullptr;
    }
}

} // extern "C"