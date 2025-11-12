// trie.cpp

#include "trie.h"
#include "tokenizer_core.h"
#include <queue>
#include <cstring>
#include <stdexcept>
#include <algorithm>

namespace auratokenizer {

    Trie::Trie() : root_(std::make_unique<Node>()), size_(0) {}

    void Trie::insert(const std::string& token_text, int token_id) {
        if (token_text.empty()) {
            throw TokenizerException("Cannot insert empty token into trie.");
        }

        Node* current = root_.get();
        for (char c : token_text) {
            if (!current->children.count(c)) {
                current->children[c] = std::make_unique<Node>();
            }
            current = current->children[c].get();
        }

        if (current->token_id == -1) {
            size_++;
        }
        current->token_id = token_id;
    }

    bool Trie::contains(const std::string& token_text) const {
        const Node* current = root_.get();
        for (char c : token_text) {
            auto it = current->children.find(c);
            if (it == current->children.end()) return false;
            current = it->second.get();
        }
        return current && current->is_token_end();
    }

    int Trie::get_id(const std::string& token_text) const {
        const Node* current = root_.get();
        for (char c : token_text) {
            auto it = current->children.find(c);
            if (it == current->children.end()) return -1;
            current = it->second.get();
        }
        return current ? current->token_id : -1;
    }

    std::pair<int, int> Trie::longest_prefix(const std::string& text, size_t start_pos) const {
        const Node* current = root_.get();
        int best_id = -1;
        int best_len = 0;

        for (size_t i = start_pos; i < text.length(); ++i) {
            char c = text[i];
            auto it = current->children.find(c);
            if (it == current->children.end()) break;
            current = it->second.get();
            if (current->is_token_end()) {
                best_id = current->token_id;
                best_len = static_cast<int>(i - start_pos + 1);
            }
        }

        return { best_id, best_len };
    }

    std::vector<std::pair<int, int>> Trie::all_prefixes(const std::string& text, size_t start_pos) const {
        std::vector<std::pair<int, int>> matches;
        const Node* current = root_.get();

        for (size_t i = start_pos; i < text.length(); ++i) {
            auto it = current->children.find(text[i]);
            if (it == current->children.end()) break;
            current = it->second.get();
            if (current->is_token_end()) {
                matches.emplace_back(current->token_id, static_cast<int>(i - start_pos + 1));
            }
        }

        return matches;
    }

    void Trie::clear() {
        root_ = std::make_unique<Node>();
        size_ = 0;
    }

    size_t Trie::size() const {
        return size_;
    }

    // --- Serialization ---

    std::vector<char> Trie::serialize() const {
        std::vector<char> buffer;
        buffer.reserve(8 * 1024); // Pre-reserve for performance

        // Write number of tokens
        size_t count = size_;
        buffer.insert(buffer.end(), reinterpret_cast<const char*>(&count), reinterpret_cast<const char*>(&count) + sizeof(size_t));

        // Start recursive serialization
        serialize_node(root_.get(), buffer);
        return buffer;
    }

    void Trie::serialize_node(const Node* node, std::vector<char>& buffer) const {
        int id = node->token_id;
        size_t num_children = node->children.size();

        buffer.insert(buffer.end(), reinterpret_cast<const char*>(&id), reinterpret_cast<const char*>(&id) + sizeof(int));
        buffer.insert(buffer.end(), reinterpret_cast<const char*>(&num_children), reinterpret_cast<const char*>(&num_children) + sizeof(size_t));

        for (const auto& [ch, child] : node->children) {
            buffer.push_back(ch);
            serialize_node(child.get(), buffer);
        }
    }

    void Trie::deserialize(const std::vector<char>& buffer) {
        clear();

        if (buffer.size() < sizeof(size_t)) {
            throw TokenizerException("Invalid trie serialization.");
        }

        const char* ptr = buffer.data();
        const char* end = buffer.data() + buffer.size();

        std::memcpy(&size_, ptr, sizeof(size_t));
        ptr += sizeof(size_t);

        root_ = deserialize_node(ptr, end);
    }

    std::unique_ptr<Trie::Node> Trie::deserialize_node(const char*& ptr, const char* end) {
        if (ptr + sizeof(int) + sizeof(size_t) > end) {
            throw TokenizerException("Invalid serialized trie structure.");
        }

        auto node = std::make_unique<Node>();

        std::memcpy(&node->token_id, ptr, sizeof(int));
        ptr += sizeof(int);

        size_t children_count = 0;
        std::memcpy(&children_count, ptr, sizeof(size_t));
        ptr += sizeof(size_t);

        for (size_t i = 0; i < children_count; ++i) {
            if (ptr + 1 > end) {
                throw TokenizerException("Unexpected end of trie data.");
            }
            char ch = *ptr++;
            node->children[ch] = deserialize_node(ptr, end);
        }

        return node;
    }

} // namespace auratokenizer
