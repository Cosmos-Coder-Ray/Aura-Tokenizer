#include "icu_integration.h"
#include <algorithm>

namespace auratokenizer {
    namespace icu {

        ICUPreTokenizer::ICUPreTokenizer(const TokenizerConfig& config)
            : config_(config) {
        }

        std::vector<std::string> ICUPreTokenizer::preTokenize(const std::string& text) const {
            if (auto special = handleSpecialCases(text); !special.empty()) {
                return special;
            }
            return icu_utils::ICUUtils::segment_words(text);
        }

        std::vector<OffsetMapping> ICUPreTokenizer::getOffsetMappings(const std::string& text) const {
            std::vector<OffsetMapping> out;
            auto toks = preTokenize(text);
            out.reserve(toks.size());

            size_t cursor = 0;
            for (const auto& tok : toks) {
                size_t pos = text.find(tok, cursor);
                if (pos == std::string::npos) {
                    out.push_back({ static_cast<int>(cursor), static_cast<int>(cursor) });
                }
                else {
                    out.push_back({ static_cast<int>(pos), static_cast<int>(pos + tok.size()) });
                    cursor = pos + tok.size();
                }
            }
            return out;
        }

        std::vector<std::string> ICUPreTokenizer::handleSpecialCases(const std::string& text) const {
            if (icu_utils::ICUUtils::is_url(text) || icu_utils::ICUUtils::is_email(text)) {
                return { text };
            }
            return {};
        }

        ICUNormalizer::ICUNormalizer(const TokenizerConfig& config)
            : config_(config) {
        }

        std::string ICUNormalizer::normalize(const std::string& text) const {
            std::string normed = icu_utils::ICUUtils::normalize(text, config_.normalization);

            if (shouldStripAccents()) {
                normed = icu_utils::ICUUtils::strip_accents(normed);
            }
            if (shouldLowercase()) {
                normed = icu_utils::ICUUtils::to_lower(normed);
            }
            return normed;
        }

        std::vector<std::string> ICUNormalizer::batchNormalize(const std::vector<std::string>& texts) const {
            std::vector<std::string> out;
            out.reserve(texts.size());
            for (const auto& s : texts) {
                out.push_back(normalize(s));
            }
            return out;
        }

        bool ICUNormalizer::shouldStripAccents() const { return config_.strip_accents; }
        bool ICUNormalizer::shouldLowercase() const { return config_.lowercase; }
    }
}