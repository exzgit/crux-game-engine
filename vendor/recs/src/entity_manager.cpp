#include "recs/entity_manager.h"

Entity EntityManager::create() {
    std::uint32_t id;

    if (!free_list.empty()) {
        // Reuse slot lama
        id = free_list.back();
        free_list.pop_back();
        slots[id].alive = true;
    } else {
        // Buat slot baru
        id = static_cast<std::uint32_t>(slots.size());
        slots.push_back({0u, true});
    }

    ++alive_entities;
    return Entity{id, slots[id].generation};
}

void EntityManager::destroy(Entity e) {
    if (e.index >= slots.size()) {
        return;
    }

    Slot& slot = slots[e.index];

    // Cegah double-destroy atau stale handle
    if (!slot.alive || slot.generation != e.generation) {
        return;
    }

    slot.alive = false;
    slot.generation++;          // invalidate semua handle lama
    free_list.push_back(e.index);
    --alive_entities;
}

bool EntityManager::is_alive(Entity e) const {
    if (e.index >= slots.size()) {
        return false;
    }

    const Slot& slot = slots[e.index];
    return slot.alive && slot.generation == e.generation;
}

std::uint32_t EntityManager::alive_count() const noexcept {
    return alive_entities;
}
