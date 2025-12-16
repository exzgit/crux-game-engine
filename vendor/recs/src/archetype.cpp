#include "recs/archetype.h"

Archetype::Archetype(ArchetypeSignature signature)
    : sig(std::move(signature)) {}

const ArchetypeSignature& Archetype::signature() const noexcept {
    return sig;
}

bool Archetype::empty() const noexcept {
    return total_entities == 0;
}

std::size_t Archetype::entity_count() const noexcept {
    return total_entities;
}

Chunk* Archetype::get_or_create_chunk() {
    if (!chunk_list.empty()) {
        Chunk* last = chunk_list.back().get();
        if (!last->full()) {
            return last;
        }
    }

    chunk_list.emplace_back(
        std::make_unique<Chunk>(sig.components())
    );
    return chunk_list.back().get();
}


std::size_t Archetype::add_entity(Entity id) {
    Chunk* chunk = get_or_create_chunk();
    std::size_t row = chunk->allocate(id);
    ++total_entities;
    return row;
}

Entity Archetype::remove_entity(std::size_t chunk_index, std::size_t row) {
    Chunk* chunk = chunk_list[chunk_index].get();
    Entity moved = chunk->deallocate(row);
    --total_entities;

    // Keep empty chunks to avoid shifting chunk indices for other entities.
    return moved;
}

const std::vector<std::unique_ptr<Chunk>>& Archetype::chunks() const noexcept {
    return chunk_list;
}
