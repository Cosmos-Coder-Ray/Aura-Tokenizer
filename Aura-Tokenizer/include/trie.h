// trie.h

#ifndef AURA_TOKENIZER_TRIE_H
#define AURA_TOKENIZER_TRIE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "tokenizer_exception.h"  // For TokenizerException

namespace auratokenizer {

    /**
     * Trie data structure to support prefix-based token lookup.
     * Optimized for use in high-performance tokenizer pipelines (BPE, Unigram, WordPiece, etc.).
     */
    class Trie {
    public:
        Trie();
        ~Trie() = default;

        void insert(const std::string& token_text, int token_id);
        bool contains(const std::string& token_text) const;
        int get_id(const std::string& token_text) const;

        // Finds the longest matching prefix from start_pos in the input text
        std::pair<int, int> longest_prefix(const std::string& text, size_t start_pos) const;

        // Finds all prefixes that match starting at a given position
        std::vector<std::pair<int, int>> all_prefixes(const std::string& text, size_t start_pos) const;

        void clear();
        size_t size() const;

        // Serialize and deserialize for model persistence
        std::vector<char> serialize() const;
        void deserialize(const std::vector<char>& data);

    private:
        struct Node {
            std::unordered_map<char, std::unique_ptr<Node>> children;
            int token_id = -1;  // -1 indicates not an end-of-token node

            bool is_token_end() const { return token_id >= 0; }
        };

        std::unique_ptr<Node> root_;
        size_t size_;  // Number of tokens in the trie

        void serialize_node(const Node* node, std::vector<char>& buffer) const;
        std::unique_ptr<Node> deserialize_node(const char*& ptr, const char* end);
    };

}  // namespace auratokenizer

#endif  // AURA_TOKENIZER_TRIE_H
