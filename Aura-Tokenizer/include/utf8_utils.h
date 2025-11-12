#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace auratokenizer {
    namespace utf8 {

        // Convert UTF-8 to Unicode codepoints (UTF-32)
        std::vector<uint32_t> utf8_to_codepoints(const std::string& utf8_str);

        // Convert Unicode codepoints (UTF-32) to UTF-8
        std::string codepoints_to_utf8(const std::vector<uint32_t>& codepoints);

        // Validate whether a string is valid UTF-8
        bool is_valid_utf8(const std::string& str);

        // Get the number of bytes in a UTF-8 sequence
        inline int sequence_length(unsigned char lead_byte) {
            if (lead_byte < 0x80) return 1;
            if (lead_byte < 0xE0) return 2;
            if (lead_byte < 0xF0) return 3;
            if (lead_byte < 0xF8) return 4;
            return 0; // Invalid lead byte
        }

        // Check if a byte is a UTF-8 continuation byte
        inline bool is_continuation_byte(unsigned char byte) {
            return (byte & 0xC0) == 0x80;
        }

    } // namespace utf8
} // namespace auratokenizer
