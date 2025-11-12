#include "template_parser.h"
#include <regex>

namespace auratokenizer {

namespace templates {

// Regex to find placeholders: {{...}} or [...] (for special tokens)
// This regex captures the content inside the brackets.
const std::regex TEMPLATE_PLACEHOLDER_REGEX(R"((\{\{[^\}]+\}\}|\[[^\]]+\]))");

ParsedTemplate TemplateParser::parse(const std::string& template_string) {
    ParsedTemplate parsed_template;
    std::sregex_iterator current_match(template_string.begin(), template_string.end(), TEMPLATE_PLACEHOLDER_REGEX);
    std::sregex_iterator last_match_end;

    int last_pos = 0;

    for (/* no init */; current_match != std::sregex_iterator(); ++current_match) {
        const std::smatch& match = *current_match;
        int match_start = match.position();
        int match_length = match.length();

        // Add literal part before the match
        if (match_start > last_pos) {
            parsed_template.push_back({
                TemplateSegment::LITERAL,
                template_string.substr(last_pos, match_start - last_pos)
            });
        }

        // Add the placeholder part
        std::string placeholder_content = match.str(1); // The captured group
        if (placeholder_content.rfind("{{ ", 0) == 0 && placeholder_content.back() == '}') { // Starts with {{ and ends with }
            // Variable placeholder: remove {{ and }}
            parsed_template.push_back({
                TemplateSegment::VARIABLE,
                placeholder_content.substr(2, placeholder_content.length() - 4) // Remove {{ and }}
            });
        } else if (placeholder_content.rfind("[", 0) == 0 && placeholder_content.back() == ']') { // Starts with [ and ends with ]
            // Special token placeholder: remove [ and ]
            parsed_template.push_back({
                TemplateSegment::SPECIAL_TOKEN,
                placeholder_content.substr(1, placeholder_content.length() - 2) // Remove [ and ]
            });
        } else {
            throw std::runtime_error("Malformed template placeholder: " + placeholder_content);
        }

        last_pos = match_start + match_length;
        last_match_end = current_match;
    }

    // Add any remaining literal part after the last match
    if (last_pos < template_string.length()) {
        parsed_template.push_back({
            TemplateSegment::LITERAL,
            template_string.substr(last_pos)
        });
    }

    return parsed_template;
}

bool TemplateParser::is_special_token_placeholder(const std::string& s) {
    // This function is currently not used by parse, but could be useful for validation
    return s.length() >= 2 && s.front() == '[' && s.back() == ']';
}

} // namespace templates

} // namespace auratokenizer
