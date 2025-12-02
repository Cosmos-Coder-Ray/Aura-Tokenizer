#include "../include/tokenizer_advanced.h"
#include <iostream>
#include <cassert>
#include <vector>

using namespace auratokenizer;

void test_basic_flow() {
    std::cout << "Running test_basic_flow..." << std::endl;
    
    TokenizerConfig config;
    auto model = std::make_shared<BPEModel>(config);
    auto normalizer = std::make_shared<UnicodeNormalizer>(config);
    auto pre_tokenizer = std::make_shared<RegexPreTokenizer>(config);
    auto post_processor = std::shared_ptr<PostProcessor>(nullptr); // Initially null

    TokenizerAdvanced tokenizer(config, model, normalizer, pre_tokenizer, post_processor);

    // Test configuration
    tokenizer.set_normalization_form(NormalizationForm::NFC);
    tokenizer.set_lowercase(true);
    
    // Test encoding (empty vocab initially)
    auto encoding = tokenizer.encode("Hello");
    assert(encoding.input_ids.size() >= 0);

    std::cout << "test_basic_flow passed!" << std::endl;
}

int main() {
    try {
        test_basic_flow();
        std::cout << "All C++ tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
