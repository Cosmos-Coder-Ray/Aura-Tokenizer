#pragma once

#include <string>
#include <vector>
#include <ostream>
#include "tokenizer_exception.h" // Include the one true definition

namespace auratokenizer {
    // Enums and simple structs only in this file
    // ... (All your enums: SpecialTokenType, TokenizationMode, etc. remain here) ...

    /**
     * @enum SpecialTokenType
     * @brief Types of special tokens used in tokenization.
     */
    enum class SpecialTokenType { UNK, PAD, BOS, EOS, MASK, SEP, CLS, BLANK, CUSTOM };
    /**
     * @enum TokenizationMode
     * @brief Modes for tokenization speed/accuracy tradeoff.
     */
    enum class TokenizationMode { NORMAL, FAST, ACCURATE, BERT_STYLE, GPT_STYLE, T5_STYLE };
    /**
     * @enum PaddingDirection
     * @brief Padding direction for sequence padding.
     */
    enum class PaddingDirection { LEFT, RIGHT };
    /**
     * @enum TruncationStrategy
     * @brief Strategies for truncating long sequences.
     */
    enum class TruncationStrategy { LONGEST_FIRST, ONLY_FIRST, ONLY_SECOND };
    /**
     * @enum NormalizationForm
     * @brief Unicode normalization forms.
     */
    enum class NormalizationForm { NONE, NFC, NFD, NFKC, NFKD };
    /**
     * @enum TokenizationAlgorithm
     * @brief Supported tokenization algorithms.
     */
    enum class TokenizationAlgorithm { BPE, UNIGRAM, WORDPIECE, CHARLEVEL };
    /**
     * @enum ModelType
     * @brief Supported model types.
     */
    enum class ModelType { BERT, GPT2, ROBERTA, XLNET, ALBERT, CUSTOM };

    // --- C-style enums for FFI ---
    extern "C" {
        typedef enum {
            C_UNK, C_PAD, C_BOS, C_EOS, C_MASK, C_SEP, C_CLS, C_BLANK, C_CUSTOM
        } C_SpecialTokenType;
        typedef enum {
            C_NORMAL, C_FAST, C_ACCURATE, C_BERT_STYLE, C_GPT_STYLE, C_T5_STYLE
        } C_TokenizationMode;
        typedef enum { C_LEFT, C_RIGHT } C_PaddingDirection;
        typedef enum { C_LONGEST_FIRST, C_ONLY_FIRST, C_ONLY_SECOND } C_TruncationStrategy;
        typedef enum { C_NONE, C_NFC, C_NFD, C_NFKC, C_NFKD } C_NormalizationForm;
        typedef enum { C_BPE, C_UNIGRAM, C_WORDPIECE, C_CHARLEVEL } C_TokenizationAlgorithm;
        typedef enum { C_BERT, C_GPT2, C_ROBERTA, C_XLNET, C_ALBERT, C_CUSTOM_MODEL } C_ModelType;
    }

    // --- Conversion functions for FFI ---
    SpecialTokenType to_cpp_special_token_type(C_SpecialTokenType val);
    C_SpecialTokenType to_c_special_token_type(SpecialTokenType val);
    TokenizationMode to_cpp_tokenization_mode(C_TokenizationMode val);
    C_TokenizationMode to_c_tokenization_mode(TokenizationMode val);
    PaddingDirection to_cpp_padding_direction(C_PaddingDirection val);
    C_PaddingDirection to_c_padding_direction(PaddingDirection val);
    TruncationStrategy to_cpp_truncation_strategy(C_TruncationStrategy val);
    C_TruncationStrategy to_c_truncation_strategy(TruncationStrategy val);
    NormalizationForm to_cpp_normalization_form(C_NormalizationForm val);
    C_NormalizationForm to_c_normalization_form(NormalizationForm val);
    TokenizationAlgorithm to_cpp_tokenization_algorithm(C_TokenizationAlgorithm val);
    C_TokenizationAlgorithm to_c_tokenization_algorithm(TokenizationAlgorithm val);
    ModelType to_cpp_model_type(C_ModelType val);
    C_ModelType to_c_model_type(ModelType val);

    struct OffsetMapping {
        int start = 0;
        int end = 0;
    };

    // --- Stream Operator Declarations ---
    // The implementations are now in tokenizer_types.cpp
    std::ostream& operator<<(std::ostream& os, const SpecialTokenType& val);
    std::ostream& operator<<(std::ostream& os, const TokenizationMode& val);
    std::ostream& operator<<(std::ostream& os, const PaddingDirection& val);
    std::ostream& operator<<(std::ostream& os, const TruncationStrategy& val);
    std::ostream& operator<<(std::ostream& os, const NormalizationForm& val);
    std::ostream& operator<<(std::ostream& os, const TokenizationAlgorithm& val);
    std::ostream& operator<<(std::ostream& os, const ModelType& val);

}