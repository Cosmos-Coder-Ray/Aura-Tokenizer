#include "unigram_trainer.h"
#include "tokenizer_types.h"    // For TokenizerException
#include <algorithm>            // std::sort, std::transform
#include <fstream>              // std::ifstream, std::ofstream
#include <stdexcept>
#include <sstream>              // std::stringstream

namespace auratokenizer {

    /*-------------------------------------------------------------------------*/
    /*  UnigramTrainer – Constructors / Configuration                           */
    /*-------------------------------------------------------------------------*/

    UnigramTrainer::UnigramTrainer()
        : min_frequency_(2),
        max_tokens_(30000),
        use_regex_(false),
        vocab_(std::make_shared<Vocab>()),
        normalizer_({})
    {
        // Initialize config_ with defaults (matching above)
        config_.min_frequency = min_frequency_;
        config_.max_tokens = max_tokens_;
        config_.use_regex = use_regex_;
        config_.regex_pattern = "";
        config_.normalization = NormalizationForm::NONE;
        config_.strip_accents = false;
        config_.lowercase = false;
        // Other config_ fields stay at defaults
    }

    UnigramTrainer::UnigramTrainer(const TokenizerConfig& c)
        : config_(c),
        min_frequency_(c.min_frequency),
        max_tokens_(c.max_tokens),
        use_regex_(c.use_regex),
        regex_pattern_(c.regex_pattern),
        vocab_(std::make_shared<Vocab>()),
        normalizer_(c)
    {
        // The UnicodeNormalizer(c) constructor uses c.normalization, c.strip_accents, c.lowercase
    }

    /*-------------------------------------------------------------------------*/
    /*  Public API – Training                                                    */
    /*-------------------------------------------------------------------------*/

    void UnigramTrainer::train(const std::vector<std::string>& texts) {
        if (texts.empty()) {
            throw TokenizerException("Empty corpus provided for UnigramTrainer::train");
        }

        // Step 1: Collect frequencies in a map
        std::unordered_map<std::string, std::size_t> frequencies;
        frequencies.reserve(texts.size() * 5);

        for (const auto& line : texts) {
            process_text(line, frequencies);
        }

        // Step 2: Build & prune the vocabulary
        build_vocab(frequencies);
    }

    void UnigramTrainer::train_from_file(const std::string& file_path) {
        std::ifstream infile(file_path);
        if (!infile.is_open()) {
            throw TokenizerException("Failed to open training file: " + file_path);
        }

        std::unordered_map<std::string, std::size_t> frequencies;
        std::string line;
        while (std::getline(infile, line)) {
            process_text(line, frequencies);
        }
        infile.close();

        build_vocab(frequencies);
    }

    void UnigramTrainer::train_from_files(const std::vector<std::string>& file_paths) {
        if (file_paths.empty()) {
            throw TokenizerException("No file paths provided for UnigramTrainer::train_from_files");
        }

        std::unordered_map<std::string, std::size_t> frequencies;
        for (const auto& path : file_paths) {
            std::ifstream infile(path);
            if (!infile.is_open()) {
                throw TokenizerException("Failed to open training file: " + path);
            }
            std::string line;
            while (std::getline(infile, line)) {
                process_text(line, frequencies);
            }
            infile.close();
        }

        build_vocab(frequencies);
    }

    /*-------------------------------------------------------------------------*/
    /*  Public API – Save / Load                                                 */
    /*-------------------------------------------------------------------------*/

    void UnigramTrainer::save(const std::string& path) {
        std::ofstream outfile(path, std::ios::binary);
        if (!outfile.is_open()) {
            throw TokenizerException("Failed to open file for writing: " + path);
        }

        // 1) Save config_
        config_.save(outfile);

        // 2) Save vocab_
        if (vocab_) {
            vocab_->save(outfile);
        }
        outfile.close();
    }

    void UnigramTrainer::load(const std::string& path) {
        std::ifstream infile(path, std::ios::binary);
        if (!infile.is_open()) {
            throw TokenizerException("Failed to open file for reading: " + path);
        }

        // 1) Load config_
        config_.load(infile);

        // Propagate config_ into internal fields
        min_frequency_ = config_.min_frequency;
        max_tokens_ = config_.max_tokens;
        use_regex_ = config_.use_regex;
        regex_pattern_ = config_.regex_pattern;
        normalizer_.set_normalization_form(config_.normalization);
        normalizer_.set_strip_accents(config_.strip_accents);
        normalizer_.set_lowercase(config_.lowercase);

        // 2) Load vocab_
        if (!vocab_) {
            vocab_ = std::make_shared<Vocab>();
        }
        vocab_->load(infile);

        infile.close();
    }

    /*-------------------------------------------------------------------------*/
    /*  Public API – Configuration Access                                        */
    /*-------------------------------------------------------------------------*/

    TokenizerConfig UnigramTrainer::get_config() const {
        return config_;
    }

    void UnigramTrainer::set_config(const TokenizerConfig& c) {
        config_ = c;
        min_frequency_ = c.min_frequency;
        max_tokens_ = c.max_tokens;
        use_regex_ = c.use_regex;
        regex_pattern_ = c.regex_pattern;

        normalizer_.set_normalization_form(c.normalization);
        normalizer_.set_strip_accents(c.strip_accents);
        normalizer_.set_lowercase(c.lowercase);
    }

    /*-------------------------------------------------------------------------*/
    /*  Public API – Unigram‐specific Setters / Getters                           */
    /*-------------------------------------------------------------------------*/

    void UnigramTrainer::set_min_frequency(std::size_t freq) {
        if (freq == 0) {
            throw TokenizerException("Minimum frequency must be >= 1");
        }
        min_frequency_ = freq;
        config_.min_frequency = freq;
    }

    void UnigramTrainer::set_max_tokens(std::size_t max) {
        if (max == 0) {
            throw TokenizerException("Maximum tokens must be >= 1");
        }
        max_tokens_ = max;
        config_.max_tokens = max;
    }

    void UnigramTrainer::set_use_regex(bool use) {
        use_regex_ = use;
        config_.use_regex = use;
    }

    void UnigramTrainer::set_regex_pattern(const std::string& pattern) {
        regex_pattern_ = pattern;
        config_.regex_pattern = pattern;
    }

    std::shared_ptr<Vocab> UnigramTrainer::get_vocab() const {
        return vocab_;
    }

    /*-------------------------------------------------------------------------*/
    /*  Private – process_text: normalize & count frequencies                   */
    /*-------------------------------------------------------------------------*/
    void UnigramTrainer::process_text(const std::string& text,
        std::unordered_map<std::string, std::size_t>& frequencies)
    {
        if (text.empty()) {
            return;
        }

        // 1) Normalize via ICU / UnicodeNormalizer
        std::string normalized = normalizer_.normalize(text);

        // 2) Tokenize
        std::vector<std::string> tokens = tokenize_text(normalized);

        // 3) Increment counts
        for (auto& tok : tokens) {
            if (!tok.empty()) {
                frequencies[tok]++;
            }
        }
    }

    /*-------------------------------------------------------------------------*/
    /*  Private – tokenize_text: character‐level or regex                        */
    /*-------------------------------------------------------------------------*/
    std::vector<std::string> UnigramTrainer::tokenize_text(const std::string& text) const {
        if (use_regex_ && !regex_pattern_.empty()) {
            // Placeholder: if you want real regex splitting, you can
            // #include <regex> here and do something like:
            // std::regex re(regex_pattern_);
            // std::sregex_token_iterator it(text.begin(), text.end(), re, -1);
            // ...
            return { text };  // For now, fallback to whole‐string token
        }

        // Default: split to single Unicode characters (UTF-8 aware)
        std::vector<std::string> tokens;
        tokens.reserve(text.size());
        for (size_t i = 0; i < text.size(); ) {
            unsigned char c = static_cast<unsigned char>(text[i]);
            int len = 1;
            if (c < 0x80) {
                len = 1;
            }
            else if ((c & 0xE0) == 0xC0) {
                len = 2;
            }
            else if ((c & 0xF0) == 0xE0) {
                len = 3;
            }
            else if ((c & 0xF8) == 0xF0) {
                len = 4;
            }
            tokens.emplace_back(text.substr(i, len));
            i += len;
        }
        return tokens;
    }

    /*-------------------------------------------------------------------------*/
    /*  Private – build_vocab: create & prune the Vocab                         */
    /*-------------------------------------------------------------------------*/
    void UnigramTrainer::build_vocab(const std::unordered_map<std::string, std::size_t>& frequencies)
    {
        // 1) Reset & add special tokens
        vocab_->clear();  // Clear existing maps
        vocab_->add_special_token(config_.unk_token, SpecialTokenType::UNK);
        vocab_->add_special_token(config_.pad_token, SpecialTokenType::PAD);
        vocab_->add_special_token(config_.bos_token, SpecialTokenType::BOS);
        vocab_->add_special_token(config_.eos_token, SpecialTokenType::EOS);
        if (!config_.mask_token.empty()) {
            vocab_->add_special_token(config_.mask_token, SpecialTokenType::MASK);
        }
        if (!config_.sep_token.empty()) {
            vocab_->add_special_token(config_.sep_token, SpecialTokenType::SEP);
        }
        if (!config_.cls_token.empty()) {
            vocab_->add_special_token(config_.cls_token, SpecialTokenType::CLS);
        }

        // 2) Collect all (token, freq) pairs that meet min_frequency_
        std::vector<std::pair<std::string, std::size_t>> candidates;
        candidates.reserve(frequencies.size());
        for (const auto& kv : frequencies) {
            if (kv.second >= min_frequency_) {
                candidates.emplace_back(kv.first, kv.second);
            }
        }

        // 3) Sort candidates by descending frequency
        std::sort(candidates.begin(), candidates.end(),
            [](auto& a, auto& b) {
                return a.second > b.second;
            });

        // 4) Insert tokens into Vocab up to max_tokens_
        std::size_t count_added = 0;
        for (const auto& p : candidates) {
            if (count_added >= max_tokens_) {
                break;
            }
            vocab_->add_token(p.first);
            count_added++;
        }
    }

} // namespace auratokenizer
