#pragma once

#include <stdexcept>
#include <string>

namespace auratokenizer {

    class TokenizerException : public std::runtime_error {
    public:
        explicit TokenizerException(const std::string& message)
            : std::runtime_error(message) {}
    };

}