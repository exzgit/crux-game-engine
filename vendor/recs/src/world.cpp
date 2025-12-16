#include "recs/world.h"

World::World() {
    locations.reserve(1024);
}

Entity World::create_entity() {
    Entity entity = entity_manager.create();

    if (entity.index >= locations.size()) {
        locations.resize(entity.index + 1);
    }

    ArchetypeSignature empty_sig;
    Archetype* archetype = archetype_manager.get_or_create(empty_sig);

    std::size_t row = archetype->add_entity(entity);

    locations[entity.index] = {
        archetype,
        archetype->chunks().size() - 1,
        row
    };

    return entity;
}

void World::destroy_entity(Entity entity) {
    if (!entity_manager.is_alive(entity)) {
        return;
    }

    auto& loc = locations[entity.index];
    Entity moved = loc.archetype->remove_entity(loc.chunk, loc.row);
    if (moved != Entity::invalid()) {
        locations[moved.index] = { loc.archetype, loc.chunk, loc.row };
    }

    entity_manager.destroy(entity);
}

bool World::alive(Entity entity) const noexcept {
    return entity_manager.is_alive(entity);
}

Query World::query() const {
    Query q;
    for (auto* archetype : archetype_manager.get_all()) {
        q.add_archetype(archetype);
    }
    return q;
}

void World::move_entity(
    Entity entity,
    Archetype* from,
    Archetype* to,
    const ArchetypeSignature&
) {
    auto& loc = locations[entity.index];

    std::size_t new_row = to->add_entity(entity);

    Chunk* src = from->chunks()[loc.chunk].get();
    Chunk* dst = to->chunks().back().get();

    src->move_entity(loc.row, *dst, new_row);

    Entity moved = from->remove_entity(loc.chunk, loc.row);
    if (moved != Entity::invalid()) {
        locations[moved.index] = { from, loc.chunk, loc.row };
    }

    loc.archetype = to;
    loc.chunk = to->chunks().size() - 1;
    loc.row = new_row;
}

void World::run_systems(float delta_time) {
    for (auto& system : systems) {
        system->run(*this, delta_time);
    }
}
