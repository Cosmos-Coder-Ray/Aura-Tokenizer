#include "utf8_utils.h"
#include <stdexcept>

namespace auratokenizer {
    namespace utf8 {

        std::vector<uint32_t> utf8_to_codepoints(const std::string& utf8_str) {
            std::vector<uint32_t> codepoints;
            codepoints.reserve(utf8_str.size());

            for (size_t i = 0; i < utf8_str.size();) {
                unsigned char lead = utf8_str[i];
                int len = sequence_length(lead);

                if (len == 0) {
                    throw std::runtime_error("Invalid UTF-8 lead byte");
                }

                if (i + len > utf8_str.size()) {
                    throw std::runtime_error("Incomplete UTF-8 sequence");
                }

                uint32_t codepoint = 0;
                switch (len) {
                case 1:
                    codepoint = lead;
                    break;
                case 2:
                    if (!is_continuation_byte(utf8_str[i + 1]))
                        throw std::runtime_error("Invalid UTF-8 continuation byte");
                    codepoint = ((lead & 0x1F) << 6) |
                        (utf8_str[i + 1] & 0x3F);
                    break;
                case 3:
                    if (!is_continuation_byte(utf8_str[i + 1]) ||
                        !is_continuation_byte(utf8_str[i + 2]))
                        throw std::runtime_error("Invalid UTF-8 continuation byte");
                    codepoint = ((lead & 0x0F) << 12) |
                        ((utf8_str[i + 1] & 0x3F) << 6) |
                        (utf8_str[i + 2] & 0x3F);
                    break;
                case 4:
                    if (!is_continuation_byte(utf8_str[i + 1]) ||
                        !is_continuation_byte(utf8_str[i + 2]) ||
                        !is_continuation_byte(utf8_str[i + 3]))
                        throw std::runtime_error("Invalid UTF-8 continuation byte");
                    codepoint = ((lead & 0x07) << 18) |
                        ((utf8_str[i + 1] & 0x3F) << 12) |
                        ((utf8_str[i + 2] & 0x3F) << 6) |
                        (utf8_str[i + 3] & 0x3F);
                    break;
                }

                codepoints.push_back(codepoint);
                i += len;
            }

            return codepoints;
        }

        std::string codepoints_to_utf8(const std::vector<uint32_t>& codepoints) {
            std::string utf8_str;
            utf8_str.reserve(codepoints.size() * 4); // Max expansion

            for (uint32_t cp : codepoints) {
                if (cp <= 0x7F) {
                    utf8_str.push_back(static_cast<char>(cp));
                }
                else if (cp <= 0x7FF) {
                    utf8_str.push_back(static_cast<char>(0xC0 | (cp >> 6)));
                    utf8_str.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
                }
                else if (cp <= 0xFFFF) {
                    utf8_str.push_back(static_cast<char>(0xE0 | (cp >> 12)));
                    utf8_str.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
                    utf8_str.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
                }
                else if (cp <= 0x10FFFF) {
                    utf8_str.push_back(static_cast<char>(0xF0 | (cp >> 18)));
                    utf8_str.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
                    utf8_str.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
                    utf8_str.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
                }
                else {
                    throw std::runtime_error("Invalid Unicode codepoint");
                }
            }

            return utf8_str;
        }

        bool is_valid_utf8(const std::string& str) {
            try {
                utf8_to_codepoints(str);
                return true;
            }
            catch (...) {
                return false;
            }
        }

    } // namespace utf8
} // namespace auratokenizer
