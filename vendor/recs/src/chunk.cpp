#include "recs/chunk.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>

Chunk::Chunk(const std::vector<ComponentTypeID>& component_types) {
    compute_layout(component_types);
    memory = static_cast<std::byte*>(std::aligned_alloc(64, CHUNK_SIZE));
}

Chunk::~Chunk() {
    std::free(memory);
}

void Chunk::compute_layout(const std::vector<ComponentTypeID>& types) {
    // debug logging removed

    // Use Structure-Of-Arrays layout: allocate a block for each component type
    // of size component_size * entity_capacity. We must pick an entity_capacity
    // such that total memory fits in CHUNK_SIZE. Start with a conservative
    // estimate and reduce if necessary.
    std::size_t per_entity_sum = 0;
    for (ComponentTypeID id : types) {
        const auto& info = ComponentRegistry::instance().info(id);
        per_entity_sum += info.size;
    }

    if (per_entity_sum == 0) {
        // No components: allow many entities (1 byte per entity)
        entity_capacity = CHUNK_SIZE;
        layouts.clear();
        entity_ids.reserve(entity_capacity);
        return;
    }

    // initial estimate
    entity_capacity = CHUNK_SIZE / per_entity_sum;
    if (entity_capacity == 0) entity_capacity = 1;

    // compute layouts and ensure total size fits; if not, reduce capacity
    while (true) {
        std::size_t offset = 0;
        for (ComponentTypeID id : types) {
            const auto& info = ComponentRegistry::instance().info(id);
            // align block start to component alignment
            offset = (offset + info.alignment - 1) & ~(info.alignment - 1);
            std::size_t block_size = info.size * entity_capacity;
            layouts[id] = { offset, info.size, &info };
            offset += block_size;
        }
        if (offset <= CHUNK_SIZE) break;
        // reduce capacity and retry
        --entity_capacity;
        if (entity_capacity == 0) {
            // should not happen, but guard
            entity_capacity = 1;
            break;
        }
    }

    entity_ids.reserve(entity_capacity);
}

std::size_t Chunk::capacity() const noexcept { return entity_capacity; }
std::size_t Chunk::size() const noexcept { return entity_count; }
bool Chunk::full() const noexcept { return entity_count >= entity_capacity; }

std::size_t Chunk::allocate(Entity id) {
    assert(!full());
    std::size_t row = entity_count++;
    // ensure entity_ids vector tracks the id for this row
    if (entity_ids.size() <= row) entity_ids.resize(row + 1);
    (void)row; (void)entity_count; (void)id; // debug data suppressed
    entity_ids[row] = id;
    return row;
}

Entity Chunk::deallocate(std::size_t row) {
    std::size_t last = entity_count - 1;
    Entity moved = Entity::invalid();
    (void)row; (void)last; (void)entity_count; (void)layouts; // debug data suppressed
    if (row != last) {
        for (auto& [_, layout] : layouts) {
            std::memcpy(
                memory + layout.offset + row * layout.stride,
                memory + layout.offset + last * layout.stride,
                layout.stride
            );
        }
        moved = entity_ids[last];
        (void)moved; (void)last; (void)row; // debug data suppressed
        entity_ids[row] = moved;
    }
    entity_ids.pop_back();
    --entity_count;
    return moved;
}

void* Chunk::component_ptr(ComponentTypeID type, std::size_t row) {
    auto it = layouts.find(type);
    if (it == layouts.end()) {
        // debug: missing layout for type; previously logged here
    }
    assert(it != layouts.end());
    return memory + it->second.offset + row * it->second.stride;
}

void Chunk::move_entity(std::size_t src_row, Chunk& dst, std::size_t& dst_row) {
    (void)src_row; (void)dst_row; (void)layouts; // debug data suppressed
    for (auto& [id, layout] : layouts) {
        std::memcpy(
            dst.component_ptr(id, dst_row),
            component_ptr(id, src_row),
            layout.stride
        );
    }
    // NOTE: do not deallocate here — caller (Archetype::remove_entity)
    // is responsible for removing the entity from the source chunk and
    // updating bookkeeping. Calling deallocate here causes double-free
    // / double-deallocation of rows.
}
