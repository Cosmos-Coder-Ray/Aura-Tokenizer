#ifndef AURATOKENIZER_C_API_H
#define AURATOKENIZER_C_API_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opaque pointers to C++ objects
typedef struct AuraTokenizer AuraTokenizer;
typedef struct AuraBPEModel AuraBPEModel;
typedef struct AuraBPETrainer AuraBPETrainer;
// ... other models and trainers (Unigram, WordPiece) would go here

// --- Error Handling ---
typedef enum {
    AURA_OK = 0,
    AURA_ERROR_NULL_POINTER,
    AURA_ERROR_INVALID_CONFIG,
    AURA_ERROR_IO,
    AURA_ERROR_TOKENIZATION,
    AURA_ERROR_TRAINER,
    AURA_ERROR_UNKNOWN,
} AuraError;

// Gets the last error message. The returned string is valid until the next API call.
// The caller should not free the returned string.
const char* aura_get_last_error_message();

// --- BPE Model ---
// Creates a BPE model from a JSON configuration string.
// Returns a new AuraBPEModel or NULL on error.
AuraBPEModel* aura_bpe_model_from_config(const char* config_json, size_t config_len);

// Destroys a BPE model object.
void aura_bpe_model_destroy(AuraBPEModel* model);

// --- BPE Trainer ---
// Creates a BPE trainer from a JSON configuration string.
AuraBPETrainer* aura_bpe_trainer_from_config(const char* config_json, size_t config_len);

// Destroys a BPE trainer object.
void aura_bpe_trainer_destroy(AuraBPETrainer* trainer);

// Feeds a corpus to the trainer (can be called multiple times).
AuraError aura_bpe_trainer_feed(AuraBPETrainer* trainer, const char* text, size_t text_len);

// Finalizes the training and returns a trained model.
// Returns a new AuraBPEModel or NULL on error.
AuraBPEModel* aura_bpe_trainer_train(AuraBPETrainer* trainer);


// --- Core Tokenizer ---
// Creates a tokenizer using a specific model.
AuraTokenizer* aura_tokenizer_create(AuraBPEModel* model); // Example with BPE

// Destroys a tokenizer object.
void aura_tokenizer_destroy(AuraTokenizer* tokenizer);

// --- Encoding/Decoding ---

// Represents a single token with its ID, value, and offsets.
typedef struct {
    uint32_t id;
    const char* value; // Points into the original string or an internal buffer
    size_t value_len;
    size_t start_offset;
    size_t end_offset;
} AuraToken;

// Represents the full encoding result.
typedef struct {
    AuraToken* tokens;
    size_t num_tokens;
} AuraEncoding;

// Frees the memory allocated for an AuraEncoding object.
void aura_encoding_destroy(AuraEncoding* encoding);

// Encodes a string into a sequence of tokens.
// The caller is responsible for calling aura_encoding_destroy on the result.
// Returns NULL on error.
AuraEncoding* aura_tokenizer_encode(AuraTokenizer* tokenizer, const char* text, size_t text_len);

// Decodes a sequence of token IDs back into a string.
// The returned string must be freed by the caller using aura_string_destroy.
// Returns NULL on error.
char* aura_tokenizer_decode(AuraTokenizer* tokenizer, const uint32_t* ids, size_t num_ids);

// Frees a string allocated by the library.
void aura_string_destroy(char* str);


#ifdef __cplusplus
}
#endif

#endif // AURATOKENIZER_C_API_H
