#pragma once

#include "tokenizer_core.h"
#include "icu_utils.h"

#include <unicode/locid.h>
#include <unicode/coll.h>
#include <unicode/brkiter.h>
#include <unicode/normalizer2.h>
#include <unicode/translit.h>
#include <unicode/uchar.h>
#include <mutex>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

namespace auratokenizer {
    namespace icu {
        // NOTE: This class is largely redundant given icu_utils and unicode_normalizer.
        // It's kept here to satisfy dependencies from your original structure,
        // but new code should prefer the other, more direct wrappers.

        class ICUPreTokenizer {
        public:
            explicit ICUPreTokenizer(const TokenizerConfig& config);
            std::vector<std::string> preTokenize(const std::string& text) const;
            std::vector<OffsetMapping> getOffsetMappings(const std::string& text) const;

        private:
            TokenizerConfig config_;
            std::vector<std::string> handleSpecialCases(const std::string& text) const;
        };

        class ICUNormalizer {
        public:
            explicit ICUNormalizer(const TokenizerConfig& config);
            std::string normalize(const std::string& text) const;
            std::vector<std::string> batchNormalize(const std::vector<std::string>& texts) const;

        private:
            TokenizerConfig config_;
            bool shouldStripAccents() const;
            bool shouldLowercase() const;
        };

    }
}