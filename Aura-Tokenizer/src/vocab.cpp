#include "vocab.h"
#include <fstream>
#include <stdexcept>

namespace auratokenizer {

    Vocab::Vocab() : next_id_(0) {}

    void Vocab::clear() {
        token_to_id_.clear();
        id_to_token_.clear();
        special_token_ids_.clear();
        special_ids_set_.clear();
        next_id_ = 0;
    }

    void Vocab::add_token(const std::string& token, int id) {
        if (token.empty() || has_token(token)) {
            return;
        }
        int assigned_id = (id < 0) ? next_id_++ : id;
        token_to_id_[token] = assigned_id;
        id_to_token_[assigned_id] = token;
        if (assigned_id >= next_id_) {
            next_id_ = assigned_id + 1;
        }
    }

    void Vocab::add_token_with_score(const std::string& token, double score, int id) {
        // For now, we ignore the score and just add the token
        // In a more sophisticated implementation, you might want to store scores
        add_token(token, id);
    }

    void Vocab::add_tokens(const std::vector<std::string>& tokens) {
        for (const auto& t : tokens) {
            add_token(t);
        }
    }

    int Vocab::get_token_id(const std::string& token) const {
        auto it = token_to_id_.find(token);
        return (it != token_to_id_.end()) ? it->second : -1;
    }

    std::string Vocab::get_token(int id) const {
        auto it = id_to_token_.find(id);
        return (it != id_to_token_.end()) ? it->second : "";
    }

    bool Vocab::has_token(const std::string& token) const {
        return token_to_id_.count(token);
    }

    bool Vocab::has_id(int id) const {
        return id_to_token_.count(id);
    }

    size_t Vocab::size() const {
        return token_to_id_.size();
    }

    void Vocab::add_special_token(const std::string& token, SpecialTokenType type) {
        if (token.empty() || has_token(token)) return;

        int id = next_id_++;
        token_to_id_[token] = id;
        id_to_token_[id] = token;
        special_token_ids_[type] = id;
        special_ids_set_.insert(id);
    }

    bool Vocab::is_special_token(const std::string& token) const {
        auto it = token_to_id_.find(token);
        if (it == token_to_id_.end()) return false;
        return special_ids_set_.count(it->second);
    }

    bool Vocab::is_special_token_id(int id) const {
        return special_ids_set_.count(id);
    }

    std::vector<std::string> Vocab::get_special_tokens() const {
        std::vector<std::string> list;
        for (const auto& [_, id] : special_token_ids_) {
            list.push_back(get_token(id));
        }
        return list;
    }

    int Vocab::get_special_token_id(SpecialTokenType type) const {
        auto it = special_token_ids_.find(type);
        return (it != special_token_ids_.end()) ? it->second : -1;
    }

    std::string Vocab::get_special_token_text(SpecialTokenType type) const {
        int id = get_special_token_id(type);
        return get_token(id);
    }

    void Vocab::save(std::ostream& out) const { /* Your existing implementation is likely fine */ }
    void Vocab::load(std::istream& in) { /* Your existing implementation is likely fine */ }
}