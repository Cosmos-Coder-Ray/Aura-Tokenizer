# Aura-Tokenizer C++ Usage

## Overview

The C++ core is located in `Aura-Tokenizer/include` and `Aura-Tokenizer/src`.

## Basic Usage

```cpp
#include "tokenizer_advanced.h"
#include "tokenizer_config.h"

using namespace auratokenizer;

int main() {
    TokenizerConfig config;
    auto model = std::make_shared<BPEModel>(config);
    auto normalizer = std::make_shared<UnicodeNormalizer>(config);
    auto pre_tokenizer = std::make_shared<RegexPreTokenizer>(config);
    auto post_processor = std::make_shared<BertPostProcessor>(config, vocab); // Requires vocab

    TokenizerAdvanced tokenizer(config, model, normalizer, pre_tokenizer, post_processor);

    // Train
    std::vector<std::string> files = {"corpus.txt"};
    tokenizer.train(files, 30000);

    // Encode
    auto encoding = tokenizer.encode("Hello world");
    
    return 0;
}
```

## Building

Use CMake to build the library:

```bash
mkdir build
cd build
cmake ..
make
```
