#include "tokenizer_python.h"
#include "vocab.h"
#include <sstream>
#include <algorithm>

namespace auratokenizer {

    PythonTokenizer::PythonTokenizer()
        : vocab_(std::make_shared<Vocab>()) {
    }

    std::vector<int> PythonTokenizer::encode(const std::string& text) const {
        if (!vocab_) {
            throw TokenizerException("Vocabulary not set");
        }

        std::vector<int> ids;
        std::istringstream iss(text);
        std::string token;
        
        while (iss >> token) {
            int id = vocab_->get_token_id(token);
            if (id != -1) {
                ids.push_back(id);
            }
        }
        
        return ids;
    }

    std::string PythonTokenizer::decode(const std::vector<int>& ids) const {
        if (!vocab_) {
            throw TokenizerException("Vocabulary not set");
        }

        std::string result;
        for (int id : ids) {
            std::string token = vocab_->get_token(id);
            if (!token.empty()) {
                result += token + " ";
            }
        }
        
        if (!result.empty() && result.back() == ' ') {
            result.pop_back();
        }
        
        return result;
    }

    std::vector<std::vector<int>> PythonTokenizer::encode_batch(const std::vector<std::string>& texts) const {
        if (!vocab_) {
            throw TokenizerException("Vocabulary not set");
        }

        std::vector<std::vector<int>> results;
        results.reserve(texts.size());
        
        for (const auto& text : texts) {
            results.push_back(encode(text));
        }
        
        return results;
    }

    std::vector<std::string> PythonTokenizer::decode_batch(const std::vector<std::vector<int>>& ids_batch) const {
        if (!vocab_) {
            throw TokenizerException("Vocabulary not set");
        }

        std::vector<std::string> results;
        results.reserve(ids_batch.size());
        
        for (const auto& ids : ids_batch) {
            results.push_back(decode(ids));
        }
        
        return results;
    }

} // namespace auratokenizer
