#include "tokenizer_core.h"
#include "icu_utils.h"
#include <sstream>

namespace auratokenizer {

    // Helper methods that could be shared by concrete tokenizer implementations
    // This is just a possible implementation. Concrete classes can override or ignore it.
    // Note: These methods are not part of the TokenizerBase interface, so they're commented out
    /*
    std::vector<std::string> TokenizerBase::split_text(const std::string& text) {
        std::vector<std::string> out;
        if (text.empty()) return out;

        std::string current_word;
        std::istringstream iss(text);
        while (iss >> current_word) {
            out.push_back(current_word);
        }
        return out;
    }

    std::string TokenizerBase::normalize_text(const std::string& text) {
        // Default: return input unchanged. Derived classes should override.
        return text;
    }
    */
}