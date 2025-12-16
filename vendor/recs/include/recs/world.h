#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <stdexcept>
#include <memory>

#include "entity.h"
#include "entity_manager.h"
#include "archetype_manager.h"
#include "component_registry.h"
#include "archetype_signature.h"
#include "system.h"
#include <new>

class World {
public:
    World();
    ~World() = default;

    // Entity lifecycle
    Entity create_entity();
    void destroy_entity(Entity entity);

    bool alive(Entity entity) const noexcept;

    // Component operations
    template<typename T>
    void add(Entity entity);

    template<typename T>
    void remove(Entity entity);

    template<typename T>
    T& get(Entity entity);

    template<typename T, typename... Args>
    void add_system(Args&&... args);

    template<typename T, typename... Args>
    void emplace(Entity entity, Args&&... args);

    void run_systems(float delta_time);

    // Query
    Query query() const;

    // Debug helpers
    void debug_print_archetypes() const;

private:
    struct EntityLocation {
        Archetype* archetype = nullptr;
        std::size_t chunk = 0;
        std::size_t row = 0;
    };

    void move_entity(
        Entity entity,
        Archetype* from,
        Archetype* to,
        const ArchetypeSignature& new_sig
    );

private:
    EntityManager entity_manager;
    ArchetypeManager archetype_manager;
    ComponentRegistry component_registry;

    std::vector<std::unique_ptr<System>> systems;
    std::vector<EntityLocation> locations;
};


template<typename T>
void World::add(Entity entity) {
    auto& loc = locations[entity.index];
    Archetype* from = loc.archetype;
    ComponentTypeID id = ComponentRegistry::instance().type_id<T>();
    // if component already present, no-op
    if (from->signature().contains(id)) return;

    ArchetypeSignature new_sig = from->signature();
    new_sig.add(id);

    Archetype* to = archetype_manager.get_or_create(new_sig);
    move_entity(entity, from, to, new_sig);
    // Construct the new component in-place using placement-new so that
    // non-trivial types (std::string, std::vector, etc.) are properly
    // constructed before user code assigns to them.
    auto& new_loc = locations[entity.index];
    void* mem = to->chunks()[new_loc.chunk]->component_ptr(id, new_loc.row);
    ::new (mem) T();
}


template<typename T>
void World::remove(Entity entity) {
    auto& loc = locations[entity.index];
    Archetype* from = loc.archetype;
    ComponentTypeID id = ComponentRegistry::instance().type_id<T>();
    // if component not present, nothing to do
    if (!from->signature().contains(id)) return;

    // Call destructor for T at the current location before moving the entity
    // to ensure non-trivial resources are released.
    void* oldmem = from->chunks()[loc.chunk]->component_ptr(id, loc.row);
    reinterpret_cast<T*>(oldmem)->~T();

    ArchetypeSignature new_sig = from->signature();
    new_sig.remove(id);

    Archetype* to = archetype_manager.get_or_create(new_sig);
    move_entity(entity, from, to, new_sig);
}

template<typename T>
T& World::get(Entity entity) {
    auto& loc = locations[entity.index];
    return loc.archetype
        ->chunks()[loc.chunk]
        ->template get<T>(loc.row);
}

template<typename T, typename... Args>
void World::add_system(Args&&... args) {
    systems.emplace_back(
        std::make_unique<T>(std::forward<Args>(args)...)
    );
}

template<typename T, typename... Args>
void World::emplace(Entity entity, Args&&... args) {
    auto& loc = locations[entity.index];
    Archetype* from = loc.archetype;
    ComponentTypeID id = ComponentRegistry::instance().type_id<T>();
    // if already contains, overwrite in-place
    if (from->signature().contains(id)) {
        T& ref = from->chunks()[loc.chunk]->template get<T>(loc.row);
        ref = T(std::forward<Args>(args)...);
        return;
    }

    ArchetypeSignature new_sig = from->signature();
    new_sig.add(id);

    Archetype* to = archetype_manager.get_or_create(new_sig);
    move_entity(entity, from, to, new_sig);

    auto& new_loc = locations[entity.index];
    void* mem = to->chunks()[new_loc.chunk]->component_ptr(id, new_loc.row);
    ::new (mem) T(std::forward<Args>(args)...);
}

inline void World::debug_print_archetypes() const {
    archetype_manager.debug_print_all();
}




