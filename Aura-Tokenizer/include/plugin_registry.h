#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <memory>

namespace auratokenizer {

/**
 * @class PluginRegistry
 * @brief Registry for custom components (pre-tokenizers, post-processors, etc.).
 *
 * Allows dynamic registration and instantiation of custom tokenizer components at runtime.
 */
class PluginRegistry {
public:
    using FactoryFunc = std::function<std::shared_ptr<void>()>;

    /**
     * @brief Register a new plugin/component factory.
     * @param name The unique name of the component.
     * @param factory The factory function to create the component.
     */
    void register_plugin(const std::string& name, FactoryFunc factory);

    /**
     * @brief Create a component by name.
     * @param name The name of the registered component.
     * @return A shared pointer to the created component, or nullptr if not found.
     */
    std::shared_ptr<void> create(const std::string& name) const;

    /**
     * @brief Get the singleton instance of the registry.
     */
    static PluginRegistry& instance();

private:
    std::unordered_map<std::string, FactoryFunc> factories_;
};

} // namespace auratokenizer 