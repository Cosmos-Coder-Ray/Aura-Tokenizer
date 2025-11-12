#include "tokenizer_types.h"

namespace auratokenizer {

    std::ostream& operator<<(std::ostream& os, const SpecialTokenType& val) {
        switch (val) {
        case SpecialTokenType::UNK:    os << "UNK"; break;
        case SpecialTokenType::PAD:    os << "PAD"; break;
        case SpecialTokenType::BOS:    os << "BOS"; break;
        case SpecialTokenType::EOS:    os << "EOS"; break;
        case SpecialTokenType::MASK:   os << "MASK"; break;
        case SpecialTokenType::SEP:    os << "SEP"; break;
        case SpecialTokenType::CLS:    os << "CLS"; break;
        case SpecialTokenType::BLANK:  os << "BLANK"; break;
        case SpecialTokenType::CUSTOM: os << "CUSTOM"; break;
        default: os << "Unknown"; break;
        }
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const TokenizationMode& val) {
        switch (val) {
        case TokenizationMode::NORMAL:     os << "NORMAL"; break;
        case TokenizationMode::FAST:       os << "FAST"; break;
        case TokenizationMode::ACCURATE:   os << "ACCURATE"; break;
        case TokenizationMode::BERT_STYLE: os << "BERT_STYLE"; break;
        case TokenizationMode::GPT_STYLE:  os << "GPT_STYLE"; break;
        case TokenizationMode::T5_STYLE:   os << "T5_STYLE"; break;
        default: os << "Unknown"; break;
        }
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const PaddingDirection& val) {
        return os << (val == PaddingDirection::LEFT ? "LEFT" : "RIGHT");
    }

    std::ostream& operator<<(std::ostream& os, const TruncationStrategy& val) {
        switch (val) {
        case TruncationStrategy::LONGEST_FIRST: os << "LONGEST_FIRST"; break;
        case TruncationStrategy::ONLY_FIRST:    os << "ONLY_FIRST"; break;
        case TruncationStrategy::ONLY_SECOND:   os << "ONLY_SECOND"; break;
        default: os << "Unknown"; break;
        }
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const NormalizationForm& val) {
        switch (val) {
        case NormalizationForm::NONE:  os << "NONE"; break;
        case NormalizationForm::NFC:   os << "NFC"; break;
        case NormalizationForm::NFD:   os << "NFD"; break;
        case NormalizationForm::NFKC:  os << "NFKC"; break;
        case NormalizationForm::NFKD:  os << "NFKD"; break;
        default: os << "Unknown"; break;
        }
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const TokenizationAlgorithm& val) {
        switch (val) {
        case TokenizationAlgorithm::BPE:        os << "BPE"; break;
        case TokenizationAlgorithm::UNIGRAM:    os << "UNIGRAM"; break;
        case TokenizationAlgorithm::WORDPIECE:  os << "WORDPIECE"; break;
        case TokenizationAlgorithm::CHARLEVEL:  os << "CHARLEVEL"; break;
        default: os << "Unknown"; break;
        }
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const ModelType& val) {
        switch (val) {
        case ModelType::BERT:    os << "BERT"; break;
        case ModelType::GPT2:    os << "GPT2"; break;
        case ModelType::ROBERTA: os << "ROBERTA"; break;
        case ModelType::XLNET:   os << "XLNET"; break;
        case ModelType::ALBERT:  os << "ALBERT"; break;
        case ModelType::CUSTOM:  os << "CUSTOM"; break;
        default: os << "Unknown"; break;
        }
        return os;
    }
}