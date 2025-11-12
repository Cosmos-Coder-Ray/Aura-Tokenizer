#include "plugin_registry.h"

namespace auratokenizer {

void PluginRegistry::register_plugin(const std::string& name, FactoryFunc factory) {
    factories_[name] = factory;
}

std::shared_ptr<void> PluginRegistry::create(const std::string& name) const {
    auto it = factories_.find(name);
    if (it != factories_.end()) {
        return it->second();
    }
    return nullptr;
}

PluginRegistry& PluginRegistry::instance() {
    static PluginRegistry instance;
    return instance;
}

} // namespace auratokenizer 