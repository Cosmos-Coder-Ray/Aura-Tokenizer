#pragma once

#include "tokenizer_types.h" 

#include <unicode/unistr.h>
#include <unicode/normalizer2.h>
#include <unicode/brkiter.h>
#include <unicode/translit.h>
#include <unicode/uchar.h>
#include <unicode/uscript.h>
#include <unicode/utypes.h>
#include <unicode/coll.h>

#include <string>
#include <vector>
#include <memory>
#include <regex>
#include <mutex>
#include <stdexcept>

namespace auratokenizer {
    namespace icu_utils {

        class ICUUtils {
        public:
            static icu::UnicodeString to_icu_string(const std::string& input);
            static std::string from_icu_string(const icu::UnicodeString& input);
            static std::string normalize(const std::string& input, NormalizationForm form);
            static std::string strip_accents(const std::string& input);
            static bool is_whitespace(UChar32 c);
            static bool is_punctuation(UChar32 c);
            static bool is_cjk(UChar32 c);
            static bool is_emoji(UChar32 c);
            static bool is_number(UChar32 c);
            static std::string to_lower(const std::string& input);
            static std::string to_upper(const std::string& input);
            static std::string to_title(const std::string& input);
            static std::vector<std::string> segment_characters(const std::string& input);
            static std::vector<std::string> segment_words(const std::string& input);
            static std::vector<std::string> segment_sentences(const std::string& input);
            static std::vector<std::string> segment_cjk(const std::string& input);
            static bool is_emoji_sequence(const std::string& input);
            static std::vector<std::string> split_emoji_sequences(const std::string& input);
            static UScriptCode detect_script(const std::string& input);
            static bool is_script(UChar32 c, UScriptCode script);
            static bool is_url(const std::string& input);
            static bool is_email(const std::string& input);
            static std::string transliterate(const std::string& input, const std::string& rules);
            static bool is_valid_utf8(const std::string& input);
            static bool is_valid_unicode(const std::string& input);

        private:
            static const icu::Normalizer2* get_normalizer(NormalizationForm form);
            // Use segment_words/segment_characters/segment_sentences for segmentation. Do not call get_break_iterator from outside this class.
            static icu::BreakIterator* get_break_iterator(UBreakIteratorType type);

            static std::unique_ptr<const icu::Normalizer2> nfc_norm_;
            static std::unique_ptr<const icu::Normalizer2> nfd_norm_;
            static std::unique_ptr<const icu::Normalizer2> nfkc_norm_;
            static std::unique_ptr<const icu::Normalizer2> nfkd_norm_;
            static std::mutex normalizer_mutex_;

            static std::unique_ptr<icu::BreakIterator> char_break_it_;
            static std::unique_ptr<icu::BreakIterator> word_break_it_;
            static std::unique_ptr<icu::BreakIterator> sentence_break_it_;
            static std::mutex break_iterator_mutex_;

            static std::unique_ptr<icu::Transliterator> accent_stripper_;
            static std::mutex transliterator_mutex_;

            static const std::regex url_regex_;
            static const std::regex email_regex_;
        };

        class UnicodeStringWrapper {
        public:
            explicit UnicodeStringWrapper(const std::string& utf8);
            std::string toUTF8() const;
        private:
            icu::UnicodeString str_;
        };

        class CollatorWrapper {
        public:
            CollatorWrapper();
            ~CollatorWrapper();
            int compare(const std::string& a, const std::string& b) const;

        private:
            icu::Collator* collator_;
            mutable std::mutex collator_mutex_;
        };

    }
}