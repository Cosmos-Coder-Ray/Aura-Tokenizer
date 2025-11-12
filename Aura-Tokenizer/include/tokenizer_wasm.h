#pragma once

#include "tokenizer_base_wasm.h"

#ifdef __EMSCRIPTEN__
    #include <emscripten/val.h>
    #include <emscripten/bind.h>
#else
    // Stub implementations for non-WASM builds
    namespace emscripten {
        class val {
        public:
            template<typename T> val(T) {}
            template<typename T> T as() const { return T(); }
        };
    }
#endif

#include <memory>
#include <string>

namespace auratokenizer {

class TokenizerWASM {
public:
    explicit TokenizerWASM(const std::string& config_json);
    
    emscripten::val encode(const std::string& text);
    emscripten::val encode_to_ids(const std::string& text);
    std::string decode(const emscripten::val& tokens);
    std::string decode_from_ids(const emscripten::val& ids);
    
    emscripten::val batch_encode(const emscripten::val& texts);
    emscripten::val batch_decode(const emscripten::val& ids);
    
    void train(const emscripten::val& texts, uint32_t vocab_size);
    void save(const std::string& path);
    void load(const std::string& path);
    
    emscripten::val get_special_tokens();
    bool is_special_token(const std::string& token);
    emscripten::val get_config();

private:
    std::shared_ptr<TokenizerBase> tokenizer_;
};

} // namespace auratokenizer