#include "vector_conversions.h"
#include <gtest/gtest.h>

namespace auratokenizer {
namespace {

TEST(VectorConversions, TokenVec) {
    std::vector<Token> tokens = {
        Token{1, "hello", 0, 5},
        Token{2, "world", 6, 11}
    };
    
    auto rust_vec = to_token_vec(tokens);
    auto cpp_vec = from_token_vec(*rust_vec);
    
    ASSERT_EQ(cpp_vec.size(), 2);
    EXPECT_EQ(cpp_vec[0].id, 1);
    EXPECT_EQ(cpp_vec[0].text, "hello");
    EXPECT_EQ(cpp_vec[1].id, 2);
    EXPECT_EQ(cpp_vec[1].text, "world");
}

TEST(VectorConversions, IdVec) {
    std::vector<int32_t> ids = {1, 2, 3};
    
    auto rust_vec = to_id_vec(ids);
    auto cpp_vec = from_id_vec(*rust_vec);
    
    ASSERT_EQ(cpp_vec.size(), 3);
    EXPECT_EQ(cpp_vec, ids);
}

TEST(VectorConversions, StringVec) {
    std::vector<std::string> strings = {"hello", "world"};
    
    auto rust_vec = to_string_vec(strings);
    auto cpp_vec = from_string_vec(*rust_vec);
    
    ASSERT_EQ(cpp_vec.size(), 2);
    EXPECT_EQ(cpp_vec, strings);
}

TEST(VectorConversions, IdVecVec) {
    std::vector<std::vector<int32_t>> vecs = {
        {1, 2, 3},
        {4, 5},
        {6}
    };
    
    auto rust_vec = to_id_vec_vec(vecs);
    auto cpp_vec = from_id_vec_vec(*rust_vec);
    
    ASSERT_EQ(cpp_vec.size(), 3);
    EXPECT_EQ(cpp_vec, vecs);
}

TEST(VectorConversions, StringVecVec) {
    std::vector<std::vector<std::string>> vecs = {
        {"hello", "world"},
        {"test"},
        {"one", "two", "three"}
    };
    
    auto rust_vec = to_string_vec_vec(vecs);
    auto cpp_vec = from_string_vec_vec(*rust_vec);
    
    ASSERT_EQ(cpp_vec.size(), 3);
    EXPECT_EQ(cpp_vec, vecs);
}

} // namespace
} // namespace auratokenizer
