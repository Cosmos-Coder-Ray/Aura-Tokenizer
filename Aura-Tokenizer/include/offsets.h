#pragma once

#include <string>
#include <vector>
#include <utility>

namespace auratokenizer {

/**
 * @struct TokenOffset
 * @brief Represents the character offset of a token in the original text.
 */
struct TokenOffset {
    int start;
    int end;
};

/**
 * @class OffsetTracker
 * @brief Tracks token and character offsets during tokenization for NER/QA alignment.
 */
class OffsetTracker {
public:
    OffsetTracker();
    ~OffsetTracker();

    /**
     * @brief Compute token offsets for a given text and its tokenized output.
     * @param text The original input text.
     * @param tokens The tokenized output (as strings).
     * @return A vector of TokenOffset structs.
     */
    std::vector<TokenOffset> compute_offsets(const std::string& text, const std::vector<std::string>& tokens);
};

} // namespace auratokenizer 