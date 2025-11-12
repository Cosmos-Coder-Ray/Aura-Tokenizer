#pragma once

#include "tokenizer_types.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <ostream>
#include <istream>

namespace auratokenizer {
    class Vocab {
    public:
        Vocab();
        virtual ~Vocab() = default;

        void add_token(const std::string& token, int id = -1);
        void add_tokens(const std::vector<std::string>& tokens);
        int get_token_id(const std::string& token) const;
        std::string get_token(int id) const;
        bool has_token(const std::string& token) const;
        bool has_id(int id) const;
        size_t size() const;

        // Additional methods for compatibility
        bool contains_token(const std::string& token) const { return has_token(token); }
        void add_token_to_vocab(const std::string& token, int id = -1) { add_token(token, id); }
        std::string get_token_text(int id) const { return get_token(id); }
        void add_token_with_score(const std::string& token, double score, int id = -1);

        void add_special_token(const std::string& token, SpecialTokenType type);
        bool is_special_token(const std::string& token) const;
        bool is_special_token_id(int id) const;
        std::vector<std::string> get_special_tokens() const;
        int get_special_token_id(SpecialTokenType type) const;
        std::string get_special_token_text(SpecialTokenType type) const;

        void save(std::ostream& out) const;
        void load(std::istream& in);

        void clear();

    protected:
        std::unordered_map<std::string, int> token_to_id_;
        std::unordered_map<int, std::string> id_to_token_;
        std::unordered_map<SpecialTokenType, int> special_token_ids_;
        std::unordered_set<int> special_ids_set_;
        int next_id_;
    };
}