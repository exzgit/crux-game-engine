#pragma once

#include <cstdint>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <cassert>

using ComponentTypeID = std::uint32_t;

struct ComponentTypeInfo {
    ComponentTypeID id;
    std::size_t size;
    std::size_t alignment;
};

class ComponentRegistry {
public:
    ComponentRegistry() = default;
    ~ComponentRegistry() = default;

    static ComponentRegistry& instance();

    template<typename T>
    ComponentTypeID type_id();

    const ComponentTypeInfo& info(ComponentTypeID id) const;

private:

    ComponentTypeID next_id = 0;
    std::unordered_map<std::type_index, ComponentTypeID> type_map;
    std::vector<ComponentTypeInfo> infos;
};

// Template implementation must be available to all translation units.
template<typename T>
ComponentTypeID ComponentRegistry::type_id() {
    auto key = std::type_index(typeid(T));
    auto it = type_map.find(key);
    if (it != type_map.end())
        return it->second;

    ComponentTypeID id = next_id++;
    type_map[key] = id;
    infos.push_back(ComponentTypeInfo{
        id,
        sizeof(T),
        alignof(T)
    });
    return id;
}
