#include "auratokenizer_c_api.h"

// --- Placeholder Implementations ---
// These are empty stubs to satisfy the linker during Rust tests.
// They don't do anything, but they provide the symbols that the linker needs to see.

const char* aura_get_last_error_message() {
    return "Placeholder C++ library is being used.";
}

AuraBPEModel* aura_bpe_model_from_config(const char* , size_t) {
    return nullptr;
}

void aura_bpe_model_destroy(AuraBPEModel*) {}

AuraBPETrainer* aura_bpe_trainer_from_config(const char* , size_t) {
    return nullptr;
}

void aura_bpe_trainer_destroy(AuraBPETrainer*) {}

AuraError aura_bpe_trainer_feed(AuraBPETrainer*, const char*, size_t) {
    return AURA_OK;
}

AuraBPEModel* aura_bpe_trainer_train(AuraBPETrainer*) {
    return nullptr;
}

AuraTokenizer* aura_tokenizer_create(AuraBPEModel*) {
    return nullptr;
}

void aura_tokenizer_destroy(AuraTokenizer*) {}

void aura_encoding_destroy(AuraEncoding*) {}

AuraEncoding* aura_tokenizer_encode(AuraTokenizer*, const char*, size_t) {
    return nullptr;
}

char* aura_tokenizer_decode(AuraTokenizer*, const uint32_t*, size_t) {
    return nullptr;
}

void aura_string_destroy(char*) {}
