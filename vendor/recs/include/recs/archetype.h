#pragma once

#include <vector>
#include <memory>
#include <cstddef>

#include "archetype_signature.h"
#include "chunk.h"
#include "entity.h"

class Archetype {
public:
    explicit Archetype(ArchetypeSignature signature);

    // Non-copyable
    Archetype(const Archetype&) = delete;
    Archetype& operator=(const Archetype&) = delete;

    // Signature
    const ArchetypeSignature& signature() const noexcept;

    // Entity storage
    bool empty() const noexcept;
    std::size_t entity_count() const noexcept;

    // Allocation / deallocation
    std::size_t add_entity(Entity id);
    Entity remove_entity(std::size_t chunk_index, std::size_t row);

    template<typename... Components, typename Func>
    void for_each(Func&& fn) {
        for (auto& up : chunk_list) {
            Chunk* chunk = up.get();
            auto arrays = chunk->get_arrays<Components...>();
            std::size_t n = chunk->size();
            for (std::size_t i = 0; i < n; ++i) {
                std::apply([&](auto... ptrs) { fn(ptrs[i]...); }, arrays);
            }
        }
    }

    // Chunk access
    const std::vector<std::unique_ptr<Chunk>>& chunks() const noexcept;

private:
    Chunk* get_or_create_chunk();

private:
    ArchetypeSignature sig;
    std::vector<std::unique_ptr<Chunk>> chunk_list;
    std::size_t total_entities = 0;
};
