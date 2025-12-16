#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <functional>

#include "component_registry.h"

class ArchetypeSignature {
public:
    ArchetypeSignature() = default;

    explicit ArchetypeSignature(std::vector<ComponentTypeID> components)
        : component_types(std::move(components)) {
        normalize();
    }

    // ---- mutasi (DIBUTUHKAN WORLD) ----

    void add(ComponentTypeID id) {
        if (!contains(id)) {
            component_types.push_back(id);
            normalize();
        }
    }

    void remove(ComponentTypeID id) {
        component_types.erase(
            std::remove(component_types.begin(), component_types.end(), id),
            component_types.end()
        );
    }

    // ---- query ----

    const std::vector<ComponentTypeID>& components() const noexcept {
        return component_types;
    }

    bool empty() const noexcept {
        return component_types.empty();
    }

    std::size_t size() const noexcept {
        return component_types.size();
    }

    bool contains(ComponentTypeID id) const noexcept {
        return std::binary_search(
            component_types.begin(),
            component_types.end(),
            id
        );
    }

    // this ⊆ other
    bool is_subset_of(const ArchetypeSignature& other) const noexcept {
        return std::includes(
            other.component_types.begin(), other.component_types.end(),
            component_types.begin(), component_types.end()
        );
    }

    // this ⊇ other
    bool is_superset_of(const ArchetypeSignature& other) const noexcept {
        return other.is_subset_of(*this);
    }

    // ---- equality ----

    bool operator==(const ArchetypeSignature& other) const noexcept {
        return component_types == other.component_types;
    }

    bool operator!=(const ArchetypeSignature& other) const noexcept {
        return !(*this == other);
    }

private:
    void normalize() {
        std::sort(component_types.begin(), component_types.end());
        component_types.erase(
            std::unique(component_types.begin(), component_types.end()),
            component_types.end()
        );
    }

private:
    std::vector<ComponentTypeID> component_types;
};

//
// Hash specialization
//
namespace std {
template<>
struct hash<ArchetypeSignature> {
    std::size_t operator()(const ArchetypeSignature& sig) const noexcept {
        std::size_t h = 0;
        for (ComponentTypeID id : sig.components()) {
            h ^= std::hash<ComponentTypeID>{}(id)
               + 0x9e3779b9
               + (h << 6)
               + (h >> 2);
        }
        return h;
    }
};
}
