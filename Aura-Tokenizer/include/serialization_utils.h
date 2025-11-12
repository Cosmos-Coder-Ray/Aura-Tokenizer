#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace auratokenizer {

    void write_primitive(std::ostream& os, const void* data, size_t size);
    void read_primitive(std::istream& is, void* data, size_t size);

    void write_string(std::ostream& os, const std::string& s);
    std::string read_string(std::istream& is);

    template<typename T>
    void write_vector(std::ostream& os, const std::vector<T>& vec) {
        size_t size = vec.size();
        write_primitive(os, &size, sizeof(size));
        if (size > 0) {
            os.write(reinterpret_cast<const char*>(vec.data()), size * sizeof(T));
        }
    }

    template<typename T>
    void read_vector(std::istream& is, std::vector<T>& vec) {
        size_t size;
        read_primitive(is, &size, sizeof(size));
        vec.resize(size);
        if (size > 0) {
            is.read(reinterpret_cast<char*>(vec.data()), size * sizeof(T));
        }
    }
}