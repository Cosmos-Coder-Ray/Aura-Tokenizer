#pragma once

#include <string>
#include <vector>
#include <variant>
#include <stdexcept>

namespace auratokenizer {

namespace templates {

// Represents a segment of the template: either a literal string or a variable/special token placeholder.
struct TemplateSegment {
    enum Type {
        LITERAL,
        VARIABLE, // e.g., {{token}}
        SPECIAL_TOKEN // e.g., [CLS]
    };

    Type type;
    std::string value; // The literal string or the name of the variable/special token

    // For special tokens, optionally store the ID if resolved during parsing
    int id = -1;
};

// Represents a parsed template, which is a sequence of segments.
using ParsedTemplate = std::vector<TemplateSegment>;

/**
 * TemplateParser
 *
 * Parses a Jinja-like template string into a sequence of TemplateSegments.
 * Supports literal text, {{variable}} placeholders, and [SPECIAL_TOKEN] placeholders.
 */
class TemplateParser {
public:
    /**
     * Parses a single template string.
     * @param template_string The Jinja-like template string (e.g., "<bos>{{message}}<eos>").
     * @return A ParsedTemplate object.
     * @throws std::runtime_error if the template string is malformed.
     */
    static ParsedTemplate parse(const std::string& template_string);

private:
    // Helper to identify special tokens (e.g., [CLS], [SEP])
    static bool is_special_token_placeholder(const std::string& s);
};

} // namespace templates

} // namespace auratokenizer
