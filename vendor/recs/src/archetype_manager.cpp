#include "recs/archetype_manager.h"

Archetype* ArchetypeManager::get_or_create(const ArchetypeSignature& signature) {
    auto it = archetypes.find(signature);
    if (it != archetypes.end()) {
        return it->second.get();
    }

    auto archetype = std::make_unique<Archetype>(signature);
    Archetype* ptr = archetype.get();
    archetypes.emplace(signature, std::move(archetype));
    return ptr;
}

Query ArchetypeManager::query(const ArchetypeSignature& required) const {
    Query q;

    for (const auto& [sig, archetype] : archetypes) {
        if (required.is_subset_of(sig)) {
            q.add_archetype(archetype.get());
        }
    }

    return q;
}

std::size_t ArchetypeManager::archetype_count() const noexcept {
    return archetypes.size();
}

std::vector<Archetype*> ArchetypeManager::get_all() const {
    std::vector<Archetype*> out;
    out.reserve(archetypes.size());
    for (const auto& [sig, archetype] : archetypes) {
        if (!archetype->empty())
            out.push_back(archetype.get());
    }
    return out;
}

void ArchetypeManager::debug_print_all() const {
    // debug logging removed
}
