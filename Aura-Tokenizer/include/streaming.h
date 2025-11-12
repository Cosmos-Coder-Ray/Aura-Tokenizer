#pragma once

#include <string>
#include <vector>
#include <memory>

namespace auratokenizer {

/**
 * @class StreamingTokenizer
 * @brief Provides memory-efficient, chunked tokenization and training for large datasets.
 *
 * This class allows processing of large text corpora in a streaming fashion, minimizing memory usage.
 * It supports incremental tokenization and training, suitable for big data and production pipelines.
 */
class StreamingTokenizer {
public:
    StreamingTokenizer();
    ~StreamingTokenizer();

    /**
     * @brief Tokenize a chunk of text, returning token IDs and offsets.
     * @param text_chunk The input text chunk.
     * @return A vector of token IDs.
     */
    std::vector<int> tokenize_chunk(const std::string& text_chunk);

    /**
     * @brief Feed a chunk of text for incremental training.
     * @param text_chunk The input text chunk.
     */
    void train_on_chunk(const std::string& text_chunk);

    /**
     * @brief Finalize training and build the vocabulary/model.
     */
    void finalize_training();
};

} // namespace auratokenizer 