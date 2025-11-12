#include "offsets.h"

namespace auratokenizer {

OffsetTracker::OffsetTracker() {}
OffsetTracker::~OffsetTracker() {}

/**
 * @brief Compute token offsets for a given text and its tokenized output.
 */
std::vector<TokenOffset> OffsetTracker::compute_offsets(const std::string& text, const std::vector<std::string>& tokens) {
    // TODO: Implement offset computation logic
    return {};
}

} // namespace auratokenizer 