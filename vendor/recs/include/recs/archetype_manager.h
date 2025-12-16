#pragma once

#include <unordered_map>
#include <memory>

#include "archetype.h"
#include "archetype_signature.h"
#include "query.h"

class ArchetypeManager {
public:
    ArchetypeManager() = default;
    ~ArchetypeManager() = default;

    // Get or create archetype
    Archetype* get_or_create(const ArchetypeSignature& signature);

    // Query archetypes by required signature
    Query query(const ArchetypeSignature& required) const;

    // Return raw pointers to all archetypes
    std::vector<Archetype*> get_all() const;

    // Debug / metrics
    std::size_t archetype_count() const noexcept;

    // Debug
    void debug_print_all() const;

private:
    std::unordered_map<
        ArchetypeSignature,
        std::unique_ptr<Archetype>
    > archetypes;
};
