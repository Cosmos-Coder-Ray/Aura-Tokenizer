#include "unicode_normalizer.h"
#include "icu_utils.h"  // for ICUUtils::normalize, strip_accents, to_lower
#include <thread>
#include <future>
#include <algorithm>
#include <stdexcept>

namespace auratokenizer {

    ////////////////////////////////////////////////////////////////////////////////
    // Constructor
    ////////////////////////////////////////////////////////////////////////////////



    ////////////////////////////////////////////////////////////////////////////////
    // normalize (single string)
    ////////////////////////////////////////////////////////////////////////////////

    std::string UnicodeNormalizer::normalize(const std::string& text) const {
        if (text.empty()) return std::string();

        // 1) Unicode normalization via ICUUtils
        std::string out = icu_utils::ICUUtils::normalize(text, config_.normalization);

        // 2) Custom transformations
        for (auto& fn : custom_transformations_) {
            out = fn(out);
        }

        // 3) Strip accents if requested
        if (config_.strip_accents) {
            out = icu_utils::ICUUtils::strip_accents(out);
        }

        // 4) Lowercase if requested
        if (config_.lowercase) {
            out = icu_utils::ICUUtils::to_lower(out);
        }

        return out;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // batchNormalize (vector of strings)
    ////////////////////////////////////////////////////////////////////////////////

    std::vector<std::string> UnicodeNormalizer::batchNormalize(
        const std::vector<std::string>& texts
    ) const {
        if (texts.empty()) return {};

        // Heuristic: if >1000 strings, parallelize
        if (texts.size() > 1000) {
            const size_t num_threads = std::max<size_t>(1, std::thread::hardware_concurrency());
            const size_t chunk_size = (texts.size() + num_threads - 1) / num_threads;

            std::vector<std::future<std::vector<std::string>>> futures;
            for (size_t i = 0; i < texts.size(); i += chunk_size) {
                futures.push_back(std::async(std::launch::async, [this, &texts, i, chunk_size]() {
                    std::vector<std::string> chunk;
                    chunk.reserve(std::min(chunk_size, texts.size() - i));
                    for (size_t j = i; j < std::min(texts.size(), i + chunk_size); ++j) {
                        chunk.push_back(normalize(texts[j]));
                    }
                    return chunk;
                    }));
            }

            std::vector<std::string> result;
            result.reserve(texts.size());
            for (auto& fut : futures) {
                auto part = fut.get();
                result.insert(result.end(), part.begin(), part.end());
            }
            return result;
        }

        // Otherwise, sequential
        std::vector<std::string> result;
        result.reserve(texts.size());
        for (auto& s : texts) {
            result.push_back(normalize(s));
        }
        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Setters / Getters for config flags
    ////////////////////////////////////////////////////////////////////////////////

    void UnicodeNormalizer::set_normalization_form(NormalizationForm form) {
        config_.normalization = form;
    }

    NormalizationForm UnicodeNormalizer::get_normalization_form() const {
        return config_.normalization;
    }

    void UnicodeNormalizer::set_strip_accents(bool strip) {
        config_.strip_accents = strip;
    }

    bool UnicodeNormalizer::shouldStripAccents() const {
        return config_.strip_accents;
    }

    void UnicodeNormalizer::set_lowercase(bool lowercase) {
        config_.lowercase = lowercase;
    }

    bool UnicodeNormalizer::shouldLowercase() const {
        return config_.lowercase;
    }



} // namespace auratokenizer