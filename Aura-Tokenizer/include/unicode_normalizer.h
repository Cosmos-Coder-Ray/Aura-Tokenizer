#pragma once

#include <string>
#include <vector>
#include <functional>
#include "tokenizer_types.h"
#include "tokenizer_config.h"  // Add missing include for TokenizerConfig
#include "icu_utils.h"   // for ICUUtils::normalize, strip_accents, to_lower

namespace auratokenizer {

    /**
     * UnicodeNormalizer
     *
     * Wraps ICUUtils so that all calls run through our central ICU layer.
     * You can add custom transformations, but core logic always uses ICUUtils.
     */
    class UnicodeNormalizer {
    public:
        explicit UnicodeNormalizer(const TokenizerConfig& config)
            : config_(config) {
        }

        /**
         * Normalize a single string:
         *   1) Apply normalization form (NFC/NFKC/etc.),
         *   2) Apply any custom transformations (in order),
         *   3) Strip accents if requested,
         *   4) Lowercase if requested.
         */
        std::string normalize(const std::string& text) const;

        /**
         * Normalize a batch of strings (parallelized for large batches).
         */
        std::vector<std::string> batchNormalize(const std::vector<std::string>& texts) const;

        void set_normalization_form(NormalizationForm form);
        NormalizationForm get_normalization_form() const;

        void set_strip_accents(bool strip);
        bool shouldStripAccents() const;

        void set_lowercase(bool lowercase);
        bool shouldLowercase() const;

        void add_custom_transformation(
            std::function<std::string(const std::string&)> transform
        ) {
            custom_transformations_.push_back(std::move(transform));
        }
        void clear_custom_transformations() {
            custom_transformations_.clear();
        }

        void set_config(const TokenizerConfig& config) {
            config_ = config;
        }

        const TokenizerConfig& get_config() const {
            return config_;
        }

    private:
        TokenizerConfig config_;
        std::vector<std::function<std::string(const std::string&)>> custom_transformations_;
    };

} // namespace auratokenizer
