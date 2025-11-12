#pragma once

#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

// Opaque pointer types for FFI
typedef struct BPETokenizer BPETokenizer;

typedef struct Token {
    int id;
    const char* text;
    int is_special;
} Token;

// TokenizerConfig struct (simplified for FFI)
typedef struct TokenizerConfig {
    const char* unk_token;
    const char* pad_token;
    const char* bos_token;
    const char* eos_token;
    const char* mask_token;
    const char* sep_token;
    const char* cls_token;
} TokenizerConfig;

// BPETokenizer C API
BPETokenizer* bpe_tokenizer_new(const TokenizerConfig* config);
void bpe_tokenizer_free(BPETokenizer* tokenizer);

// Encode/decode
size_t bpe_tokenizer_encode(const BPETokenizer* tokenizer, const char* text, Token* out_tokens, size_t max_tokens);
size_t bpe_tokenizer_encode_to_ids(const BPETokenizer* tokenizer, const char* text, int* out_ids, size_t max_ids);
size_t bpe_tokenizer_batch_encode(const BPETokenizer* tokenizer, const char** texts, size_t num_texts, int** out_ids, size_t* out_lens);
size_t bpe_tokenizer_decode(const BPETokenizer* tokenizer, const Token* tokens, size_t num_tokens, char* out_text, size_t max_len);
size_t bpe_tokenizer_decode_from_ids(const BPETokenizer* tokenizer, const int* ids, size_t num_ids, char* out_text, size_t max_len);

// Training, saving, loading
void bpe_tokenizer_train(BPETokenizer* tokenizer, const char** corpus, size_t num_texts, size_t vocab_size);
void bpe_tokenizer_save(const BPETokenizer* tokenizer, const char* path);
void bpe_tokenizer_load(BPETokenizer* tokenizer, const char* path);

#ifdef __cplusplus
}
#endif
