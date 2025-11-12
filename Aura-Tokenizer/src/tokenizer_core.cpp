// tokenizer_core.cpp
#include "tokenizer_core.h"
#include "serialization_utils.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

namespace auratokenizer {

    // Default implementation of TokenizerBase methods
    void TokenizerBase::set_config(const TokenizerConfig& config) {
        config_ = config;
    }

    const TokenizerConfig& TokenizerBase::get_config() const {
        return config_;
    }

} // namespace auratokenizer
