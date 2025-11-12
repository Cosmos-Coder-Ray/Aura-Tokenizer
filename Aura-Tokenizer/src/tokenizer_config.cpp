#include "tokenizer_config.h"
#include "tokenizer_exception.h"
#include <sstream>

// The nlohmann/json.hpp dependency is needed for load_from_json_string.
// You must download the json.hpp header and add it to your include path.
// For now, this is commented out to allow compilation.
// #include "nlohmann/json.hpp"
// using json = nlohmann::json;

namespace auratokenizer {
    void TokenizerConfig::write_string(std::ostream& out, const std::string& str) const {
        size_t len = str.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        if (len > 0) out.write(str.c_str(), static_cast<std::streamsize>(len));
    }

    std::string TokenizerConfig::read_string(std::istream& in) {
        size_t size = 0;
        in.read(reinterpret_cast<char*>(&size), sizeof(size));
        if (!in) { throw TokenizerException("Failed to read string size from stream."); }
        if (size == 0) return "";
        std::string str(size, '\0');
        in.read(&str[0], size);
        if (!in) { throw TokenizerException("Failed to read string data from stream."); }
        return str;
    }

    void TokenizerConfig::save(std::ostream& out) const {
        // Implement full serialization for all members
        write_string(out, model_type);
        // ... and so on for all other members
    }

    void TokenizerConfig::load(std::istream& in) {
        // Implement full deserialization for all members
        model_type = read_string(in);
        // ... and so on for all other members
    }

    void TokenizerConfig::load_from_json_string(const std::string& json_str) {
        // This function requires the nlohmann/json.hpp library.
        // It's commented out to allow your project to build.
        // To use it, get the header and uncomment this section.
        /*
        try {
            json j = json::parse(json_str);
            vocab_size = j.value("vocab_size", vocab_size);
            model_type = j.value("model_type", model_type);
            // ... and so on for all other members
        } catch (const json::parse_error& e) {
            throw TokenizerException("Failed to parse JSON config: " + std::string(e.what()));
        }
        */
        (void)json_str; // Suppress unused parameter warning
    }

    std::string TokenizerConfig::to_string() const {
        std::ostringstream oss;
        oss << "TokenizerConfig{";
        oss << "vocab_size=" << vocab_size << ", ";
        oss << "model_type=\"" << model_type << "\", ";
        oss << "add_special_tokens=" << (add_special_tokens ? "true" : "false") << ", ";
        oss << "add_byte_fallback=" << (add_byte_fallback ? "true" : "false") << ", ";
        oss << "unk_token=\"" << unk_token << "\", ";
        oss << "pad_token=\"" << pad_token << "\", ";
        oss << "bos_token=\"" << bos_token << "\", ";
        oss << "eos_token=\"" << eos_token << "\", ";
        oss << "mask_token=\"" << mask_token << "\", ";
        oss << "sep_token=\"" << sep_token << "\", ";
        oss << "cls_token=\"" << cls_token << "\", ";
        oss << "lowercase=" << (lowercase ? "true" : "false") << ", ";
        oss << "strip_accents=" << (strip_accents ? "true" : "false") << ", ";
        oss << "normalize_whitespace=" << (normalize_whitespace ? "true" : "false") << ", ";
        oss << "remove_control_chars=" << (remove_control_chars ? "true" : "false") << ", ";
        oss << "remove_diacritics=" << (remove_diacritics ? "true" : "false") << ", ";
        oss << "min_frequency=" << min_frequency << ", ";
        oss << "max_tokens=" << max_tokens << ", ";
        oss << "use_regex=" << (use_regex ? "true" : "false") << ", ";
        oss << "regex_pattern=\"" << regex_pattern << "\", ";
        oss << "max_length=" << max_length << ", ";
        oss << "pad_to_max_length=" << (pad_to_max_length ? "true" : "false");
        oss << "}";
        return oss.str();
    }

    TokenizerConfig TokenizerConfig::from_string(const std::string& str) {
        TokenizerConfig config;
        
        // Simple parsing - in practice you'd want more robust parsing
        // This is a basic implementation that looks for key=value pairs
        size_t pos = 0;
        while (pos < str.length()) {
            size_t start = str.find('=', pos);
            if (start == std::string::npos) break;
            
            size_t key_start = str.rfind(' ', start);
            if (key_start == std::string::npos || key_start >= start) {
                pos = start + 1;
                continue;
            }
            
            std::string key = str.substr(key_start + 1, start - key_start - 1);
            size_t value_end = str.find(',', start);
            if (value_end == std::string::npos) value_end = str.find('}', start);
            if (value_end == std::string::npos) break;
            
            std::string value = str.substr(start + 1, value_end - start - 1);
            
            // Remove quotes if present
            if (value.length() >= 2 && value[0] == '"' && value[value.length()-1] == '"') {
                value = value.substr(1, value.length() - 2);
            }
            
            // Parse based on key
            if (key == "vocab_size") {
                config.vocab_size = std::stoul(value);
            } else if (key == "model_type") {
                config.model_type = value;
            } else if (key == "add_special_tokens") {
                config.add_special_tokens = (value == "true");
            } else if (key == "add_byte_fallback") {
                config.add_byte_fallback = (value == "true");
            } else if (key == "unk_token") {
                config.unk_token = value;
            } else if (key == "pad_token") {
                config.pad_token = value;
            } else if (key == "bos_token") {
                config.bos_token = value;
            } else if (key == "eos_token") {
                config.eos_token = value;
            } else if (key == "mask_token") {
                config.mask_token = value;
            } else if (key == "sep_token") {
                config.sep_token = value;
            } else if (key == "cls_token") {
                config.cls_token = value;
            } else if (key == "lowercase") {
                config.lowercase = (value == "true");
            } else if (key == "strip_accents") {
                config.strip_accents = (value == "true");
            } else if (key == "normalize_whitespace") {
                config.normalize_whitespace = (value == "true");
            } else if (key == "remove_control_chars") {
                config.remove_control_chars = (value == "true");
            } else if (key == "remove_diacritics") {
                config.remove_diacritics = (value == "true");
            } else if (key == "min_frequency") {
                config.min_frequency = std::stoul(value);
            } else if (key == "max_tokens") {
                config.max_tokens = std::stoul(value);
            } else if (key == "use_regex") {
                config.use_regex = (value == "true");
            } else if (key == "regex_pattern") {
                config.regex_pattern = value;
            } else if (key == "max_length") {
                config.max_length = std::stoi(value);
            } else if (key == "pad_to_max_length") {
                config.pad_to_max_length = (value == "true");
            }
            
            pos = value_end + 1;
        }
        
        return config;
    }

    void TokenizerConfig::add_special_token(const std::string& token, int id) {
        added_tokens[token] = std::to_string(id);
    }

}