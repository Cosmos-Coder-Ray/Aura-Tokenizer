#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include "tokenizer_exception.h" // For TokenizerException

namespace auratokenizer {

    class DoubleArrayTrie {
    public:
        struct Node {
            int32_t base;
            int32_t check;
            int32_t value;

            Node() : base(0), check(-1), value(-1) {}
        };

        DoubleArrayTrie();
        ~DoubleArrayTrie() = default;

        DoubleArrayTrie(const DoubleArrayTrie&) = delete;
        DoubleArrayTrie& operator=(const DoubleArrayTrie&) = delete;
        DoubleArrayTrie(DoubleArrayTrie&&) noexcept = default;
        DoubleArrayTrie& operator=(DoubleArrayTrie&&) noexcept = default;

        void build(const std::vector<std::pair<std::string, int32_t>>& entries);
        int32_t find(const std::string& key) const;
        std::vector<std::pair<std::string, int32_t>> common_prefix_search(const std::string& key) const;
        std::vector<std::pair<std::string, int32_t>> predictive_search(const std::string& prefix) const;
        void optimize();
        void clear();

        size_t size() const;
        size_t capacity() const;

        void rebuild_fail_links();
        void compact();
        void shrink_to_fit();

        std::vector<char> serialize() const;
        void deserialize(const std::vector<char>& data);

        void validate() const;
        void print_stats() const;

    private:
        std::vector<Node>    nodes_;
        std::vector<int32_t> fail_links_;
        std::vector<int32_t> free_list_;

        size_t size_;
        size_t capacity_;

        static constexpr int32_t ROOT_NODE = 0;
        static constexpr int32_t INVALID_NODE = -1;
        static constexpr size_t INITIAL_CAPACITY = 1024;

        int32_t find_base(const std::vector<unsigned char>& chars);
        void resize(size_t new_capacity);
    };

}