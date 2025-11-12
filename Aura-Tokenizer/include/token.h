#pragma once
#include <string>
#include "tokenizer_types.h" // For OffsetMapping

namespace auratokenizer {
    struct Token {
        int id;
        std::string text;
        bool is_special;
        OffsetMapping offset;

        Token(int p_id = -1, const std::string& p_text = "", bool p_is_special = false, OffsetMapping p_offset = { 0,0 })
            : id(p_id), text(p_text), is_special(p_is_special), offset(p_offset) {
        }
    };
}