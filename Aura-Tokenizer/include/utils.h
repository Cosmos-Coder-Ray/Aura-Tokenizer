#ifndef AURATOKENIZER_UTILS_H
#define AURATOKENIZER_UTILS_H

#include <vector>
#include <string>
#include <unordered_map>
#include <utility>

namespace auratokenizer {

    template<typename K, typename V>
    inline std::vector<std::pair<K, V>> map_to_vec(const std::unordered_map<K, V>& map) {
        return std::vector<std::pair<K, V>>(map.begin(), map.end());
    }

    inline std::vector<std::pair<std::string, int>> map_to_vec(const std::unordered_map<std::string, int>& m) {
        return map_to_vec<std::string, int>(m);
    }

    inline std::vector<std::pair<std::string, float>> map_to_vec_float(const std::unordered_map<std::string, float>& m) {
        return map_to_vec<std::string, float>(m);
    }

} // namespace auratokenizer

#endif // AURATOKENIZER_UTILS_H
