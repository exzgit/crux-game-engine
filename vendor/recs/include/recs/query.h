#pragma once

#include <vector>
#include "recs/archetype.h"

class Query {
public:
    Query() = default;

    explicit Query(const std::vector<Archetype*>& archetypes)
        : matched(archetypes) {}

    void add_archetype(Archetype* archetype) {
        matched.push_back(archetype);
    }

    template <typename... Components, typename Func>
    void for_each(Func&& fn) {
        for (Archetype* archetype : matched) {
            // Skip empty archetypes (no entities) quickly.
            if (archetype->empty()) continue;

            // debug logging removed
            const auto& sig = archetype->signature();

            // Only run the callback on archetypes that actually contain all
            // requested component types.
            bool contains_all = (sig.contains(ComponentRegistry::instance().type_id<Components>()) && ...);
            if (contains_all) {
                archetype->template for_each<Components...>(fn);
            } else {
                // archetype does not match the requested components; skip silently
                // (avoid excessive noise for archetypes that legitimately lack components)
            }
        }
    }

private:
    std::vector<Archetype*> matched;
};
