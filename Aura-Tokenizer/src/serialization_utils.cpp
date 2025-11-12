#include "serialization_utils.h"
#include "tokenizer_exception.h"

namespace auratokenizer {

    void write_primitive(std::ostream& os, const void* data, size_t size) {
        os.write(reinterpret_cast<const char*>(data), size);
        if (!os) throw TokenizerException("Failed to write primitive to stream.");
    }

    void read_primitive(std::istream& is, void* data, size_t size) {
        is.read(reinterpret_cast<char*>(data), size);
        if (!is) throw TokenizerException("Failed to read primitive from stream.");
    }

    void write_string(std::ostream& os, const std::string& s) {
        size_t length = s.size();
        write_primitive(os, &length, sizeof(length));
        if (length > 0) os.write(s.data(), length);
        if (!os) throw TokenizerException("Failed to write string to stream.");
    }

    std::string read_string(std::istream& is) {
        size_t length = 0;
        read_primitive(is, &length, sizeof(length));
        if (!is || length == 0) return "";

        std::string s(length, '\0');
        if (length > 0) is.read(&s[0], length);
        if (!is) throw TokenizerException("Failed to read string from stream.");

        return s;
    }
}