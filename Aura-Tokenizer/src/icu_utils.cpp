#include "icu_utils.h"
#include <unicode/uclean.h>
#include <mutex>

namespace auratokenizer {
    namespace icu_utils {

        std::unique_ptr<const icu::Normalizer2> ICUUtils::nfc_norm_;
        std::unique_ptr<const icu::Normalizer2> ICUUtils::nfd_norm_;
        std::unique_ptr<const icu::Normalizer2> ICUUtils::nfkc_norm_;
        std::unique_ptr<const icu::Normalizer2> ICUUtils::nfkd_norm_;
        std::mutex ICUUtils::normalizer_mutex_;
        std::unique_ptr<icu::BreakIterator> ICUUtils::char_break_it_;
        std::unique_ptr<icu::BreakIterator> ICUUtils::word_break_it_;
        std::unique_ptr<icu::BreakIterator> ICUUtils::sentence_break_it_;
        std::mutex ICUUtils::break_iterator_mutex_;
        std::unique_ptr<icu::Transliterator> ICUUtils::accent_stripper_;
        std::mutex ICUUtils::transliterator_mutex_;

        const std::regex ICUUtils::url_regex_(R"((http|https)://[a-zA-Z0-9./\-_?=&%]+)");
        const std::regex ICUUtils::email_regex_(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");


        icu::UnicodeString ICUUtils::to_icu_string(const std::string& input) {
            return icu::UnicodeString::fromUTF8(input);
        }

        std::string ICUUtils::from_icu_string(const icu::UnicodeString& input) {
            std::string out;
            input.toUTF8String(out);
            return out;
        }

        const icu::Normalizer2* ICUUtils::get_normalizer(NormalizationForm form) {
            std::lock_guard<std::mutex> lock(normalizer_mutex_);
            UErrorCode status = U_ZERO_ERROR;
            switch (form) {
            case NormalizationForm::NFC:
                if (!nfc_norm_) nfc_norm_.reset(icu::Normalizer2::getNFCInstance(status));
                if (U_FAILURE(status)) throw TokenizerException("Failed to get NFC normalizer");
                return nfc_norm_.get();
            case NormalizationForm::NFD:
                if (!nfd_norm_) nfd_norm_.reset(icu::Normalizer2::getNFDInstance(status));
                if (U_FAILURE(status)) throw TokenizerException("Failed to get NFD normalizer");
                return nfd_norm_.get();
            case NormalizationForm::NFKC:
                if (!nfkc_norm_) nfkc_norm_.reset(icu::Normalizer2::getNFKCInstance(status));
                if (U_FAILURE(status)) throw TokenizerException("Failed to get NFKC normalizer");
                return nfkc_norm_.get();
            case NormalizationForm::NFKD:
                if (!nfkd_norm_) nfkd_norm_.reset(icu::Normalizer2::getNFKDInstance(status));
                if (U_FAILURE(status)) throw TokenizerException("Failed to get NFKD normalizer");
                return nfkd_norm_.get();
            default:
                return nullptr;
            }
        }

        icu::BreakIterator* ICUUtils::get_break_iterator(UBreakIteratorType type) {
            std::lock_guard<std::mutex> lock(break_iterator_mutex_);
            UErrorCode status = U_ZERO_ERROR;
            switch (type) {
            case UBRK_CHARACTER:
                if (!char_break_it_) char_break_it_.reset(icu::BreakIterator::createCharacterInstance(icu::Locale::getDefault(), status));
                if (U_FAILURE(status)) throw TokenizerException("Failed to create char break iterator");
                return char_break_it_.get();
            case UBRK_WORD:
                if (!word_break_it_) word_break_it_.reset(icu::BreakIterator::createWordInstance(icu::Locale::getDefault(), status));
                if (U_FAILURE(status)) throw TokenizerException("Failed to create word break iterator");
                return word_break_it_.get();
            case UBRK_SENTENCE:
                if (!sentence_break_it_) sentence_break_it_.reset(icu::BreakIterator::createSentenceInstance(icu::Locale::getDefault(), status));
                if (U_FAILURE(status)) throw TokenizerException("Failed to create sentence break iterator");
                return sentence_break_it_.get();
            default:
                return nullptr;
            }
        }

        std::string ICUUtils::normalize(const std::string& input, NormalizationForm form) {
            const icu::Normalizer2* normalizer = get_normalizer(form);
            if (!normalizer) return input;
            UErrorCode status = U_ZERO_ERROR;
            icu::UnicodeString ustr = to_icu_string(input);
            icu::UnicodeString out;
            normalizer->normalize(ustr, out, status);
            if (U_FAILURE(status)) throw TokenizerException("ICU normalization failed");
            return from_icu_string(out);
        }

        std::string ICUUtils::strip_accents(const std::string& input) {
            std::lock_guard<std::mutex> lock(transliterator_mutex_);
            if (!accent_stripper_) {
                UErrorCode status = U_ZERO_ERROR;
                accent_stripper_.reset(
                    icu::Transliterator::createInstance("Any-Latin; NFD; [:Nonspacing Mark:] Remove; NFC", UTRANS_FORWARD, status));
                if (U_FAILURE(status) || !accent_stripper_) throw TokenizerException("Failed to create ICU accent-stripping Transliterator");
            }
            icu::UnicodeString ustr = to_icu_string(input);
            accent_stripper_->transliterate(ustr);
            return from_icu_string(ustr);
        }

        bool ICUUtils::is_whitespace(UChar32 c) { return u_isspace(c); }
        bool ICUUtils::is_punctuation(UChar32 c) { return u_ispunct(c); }
        bool ICUUtils::is_cjk(UChar32 c) {
            UErrorCode status = U_ZERO_ERROR;
            UScriptCode script = uscript_getScript(c, &status);
            return !U_FAILURE(status) && (script == USCRIPT_HAN || script == USCRIPT_HIRAGANA || script == USCRIPT_KATAKANA);
        }
        bool ICUUtils::is_emoji(UChar32 c) { return u_hasBinaryProperty(c, UCHAR_EMOJI); }
        bool ICUUtils::is_number(UChar32 c) { return u_isdigit(c); }

        std::string ICUUtils::to_lower(const std::string& input) {
            icu::UnicodeString ustr = to_icu_string(input);
            ustr.toLower();
            return from_icu_string(ustr);
        }

        std::string ICUUtils::to_upper(const std::string& input) {
            icu::UnicodeString ustr = to_icu_string(input);
            ustr.toUpper();
            return from_icu_string(ustr);
        }

        std::string ICUUtils::to_title(const std::string& input) {
            icu::UnicodeString ustr = to_icu_string(input);
            ustr.toTitle(nullptr);
            return from_icu_string(ustr);
        }

        std::vector<std::string> ICUUtils::segment_characters(const std::string& input) { 
            icu::BreakIterator* iter = get_break_iterator(UBRK_CHARACTER);
            icu::UnicodeString ustr = to_icu_string(input);
            iter->setText(ustr);

            std::vector<std::string> segments;
            int32_t start = iter->first();
            for (int32_t end = iter->next(); end != icu::BreakIterator::DONE; start = end, end = iter->next()) {
                icu::UnicodeString segment_ustr;
                ustr.extract(start, end - start, segment_ustr);
                segments.push_back(from_icu_string(segment_ustr));
            }
            return segments;
        }
        
        std::vector<std::string> ICUUtils::segment_words(const std::string& input) { 
            icu::BreakIterator* iter = get_break_iterator(UBRK_WORD);
            icu::UnicodeString ustr = to_icu_string(input);
            iter->setText(ustr);

            std::vector<std::string> segments;
            int32_t start = iter->first();
            for (int32_t end = iter->next(); end != icu::BreakIterator::DONE; start = end, end = iter->next()) {
                icu::UnicodeString segment_ustr;
                ustr.extract(start, end - start, segment_ustr);
                segments.push_back(from_icu_string(segment_ustr));
            }
            return segments;
        }
        
        std::vector<std::string> ICUUtils::segment_sentences(const std::string& input) { 
            icu::BreakIterator* iter = get_break_iterator(UBRK_SENTENCE);
            icu::UnicodeString ustr = to_icu_string(input);
            iter->setText(ustr);

            std::vector<std::string> segments;
            int32_t start = iter->first();
            for (int32_t end = iter->next(); end != icu::BreakIterator::DONE; start = end, end = iter->next()) {
                icu::UnicodeString segment_ustr;
                ustr.extract(start, end - start, segment_ustr);
                segments.push_back(from_icu_string(segment_ustr));
            }
            return segments;
        }

        std::vector<std::string> ICUUtils::segment_cjk(const std::string& input) {
            // A simplified logic. Proper CJK segmentation may require dictionaries.
            std::vector<std::string> result;
            icu::UnicodeString ustr = to_icu_string(input);
            std::string current_non_cjk;
            for (int32_t i = 0; i < ustr.length();) {
                UChar32 c = ustr.char32At(i);
                if (is_cjk(c)) {
                    if (!current_non_cjk.empty()) {
                        result.push_back(current_non_cjk);
                        current_non_cjk.clear();
                    }
                    icu::UnicodeString cjk_char(c);
                    result.push_back(from_icu_string(cjk_char));
                }
                else {
                    current_non_cjk += from_icu_string(icu::UnicodeString(c));
                }
                i += U16_LENGTH(c);
            }
            if (!current_non_cjk.empty()) {
                result.push_back(current_non_cjk);
            }
            return result;
        }

        bool ICUUtils::is_url(const std::string& input) { return std::regex_match(input, url_regex_); }
        bool ICUUtils::is_email(const std::string& input) { return std::regex_match(input, email_regex_); }

        // --- Other wrapper classes ---
        UnicodeStringWrapper::UnicodeStringWrapper(const std::string& utf8) { str_ = ICUUtils::to_icu_string(utf8); }
        std::string UnicodeStringWrapper::toUTF8() const { return ICUUtils::from_icu_string(str_); }

        CollatorWrapper::CollatorWrapper() {
            UErrorCode status = U_ZERO_ERROR;
            collator_ = icu::Collator::createInstance(status);
            if (U_FAILURE(status) || !collator_) throw TokenizerException("Failed to create ICU Collator");
        }
        CollatorWrapper::~CollatorWrapper() { delete collator_; }

        int CollatorWrapper::compare(const std::string& a, const std::string& b) const {
            std::lock_guard<std::mutex> lock(collator_mutex_);
            UErrorCode status = U_ZERO_ERROR;
            return collator_->compare(ICUUtils::to_icu_string(a), ICUUtils::to_icu_string(b), status);
        }

    }
}